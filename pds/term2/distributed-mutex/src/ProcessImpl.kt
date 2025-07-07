package mutex

/**
 * Distributed mutual exclusion implementation.
 * All functions are called from the single main thread.
 *
 * @author Artyom Fadeyev
 */
class ProcessImpl(
    private val env: Environment,
) : Process {
    private val forks: MutableList<TFork> = MutableList(env.nProcesses + 1) { TFork.DIRTY }
    private val requests: MutableList<Boolean> = MutableList(env.nProcesses + 1) { false }
    private var locked = false
    private var hungry = false

    init {
        // only single process can take the first fork to avoid deadlock
        ((env.processId + 1) until (env.nProcesses + 1)).forEach { forks[it] = TFork.MISSING }
    }

    override fun onMessage(
        srcId: Int,
        message: Message,
    ) {
        message.parse {
            when (readEnum<TMessage>()) {
                TMessage.REQ -> {
                    if (locked || forks[srcId] != TFork.DIRTY) {
                        requests[srcId] = true
                        return
                    }

                    forks[srcId] = TFork.MISSING
                    if (hungry) {
                        env.send(srcId) { writeEnum(TMessage.REQ) }
                    }

                    env.send(srcId) { writeEnum(TMessage.REL) }
                }

                TMessage.REL -> {
                    forks[srcId] = TFork.CLEAN
                    checkCSEnter()
                }
            }
        }
    }

    override fun onLockRequest() {
        hungry = true
        if (checkCSEnter()) {
            return
        }

        forks.forEachIndexed { destId, fork ->
            if (fork == TFork.MISSING) {
                env.send(destId) { writeEnum(TMessage.REQ) }
            }
        }
    }

    override fun onUnlockRequest() {
        env.unlocked()

        forks.fill(TFork.DIRTY)
        locked = false
        hungry = false

        forks.indices
            .filter { requests[it] }
            .forEach { destId ->
                forks[destId] = TFork.MISSING
                env.send(destId) { writeEnum(TMessage.REL) }
                requests[destId] = false
            }
    }

    private fun checkCSEnter(): Boolean {
        if (!hungry || forks.any { it == TFork.MISSING }) {
            return false
        }

        env.locked()
        locked = true
        return true
    }

    enum class TMessage { REQ, REL }

    enum class TFork { CLEAN, DIRTY, MISSING }
}
