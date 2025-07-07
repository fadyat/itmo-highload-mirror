package solution

sealed class Message : java.io.Serializable

object AckMessage : Message()

object RemoveChildMessage : Message()

object AddChildMessage : Message()

data class DistanceMessage(
    val distance: Long,
) : Message()

/**
 * Distributed Dijkstra algorithm implementation.
 * All functions are called from the single main thread.
 *
 * @author Artyom Fadeyev
 */
class DijkstraProcessImpl(
    private val env: Environment,
) : DijkstraProcess {
    private var isLeader = false
    private var minDistance: Long? = null
    private var parentId: Int? = null
    private var ackBalance: Long = 0
    private var childrenCount: Long = 0

    override fun onComputationStart() {
        isLeader = true
        minDistance = 0
        updateNeighbours()
        tryToComplete()
    }

    private fun updateNeighbours() {
        for ((neighbourId, weight) in env.neighbours) {
            if (neighbourId != env.processId) {
                env.send(neighbourId, DistanceMessage(minDistance!! + weight))
                ackBalance++
            }
        }
    }

    private fun tryToComplete() {
        if (ackBalance == 0L && childrenCount == 0L) {
            if (isLeader) {
                env.finishExecution()
            } else if (parentId != null) {
                env.send(parentId!!, RemoveChildMessage)
                parentId = null
            }
        }
    }

    override fun onMessage(
        srcId: Int,
        message: Any,
    ) {
        when {
            message is RemoveChildMessage -> childrenCount--
            message is AddChildMessage -> {
                ackBalance--
                childrenCount++
            }
            message is AckMessage -> ackBalance--
            message is DistanceMessage -> {
                if (minDistance == null || message.distance < minDistance!!) {
                    if (parentId != null) {
                        env.send(parentId!!, RemoveChildMessage)
                    }

                    parentId = srcId
                    minDistance = message.distance
                    env.send(srcId, AddChildMessage)
                    updateNeighbours()
                } else {
                    env.send(srcId, AckMessage)
                }
            }
        }

        tryToComplete()
    }

    override val distance: Long?
        get() = minDistance
}
