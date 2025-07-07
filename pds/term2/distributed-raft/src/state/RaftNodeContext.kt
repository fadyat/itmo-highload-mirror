package raft.state

import raft.*
import raft.Message.*
import java.util.LinkedList
import kotlin.math.max

sealed class BaseState(val ctx: RaftNodeContext) {
    var persistentState =
        ctx.env.storage.readPersistentState()
        set(value) {
            ctx.env.storage.writePersistentState(value)
            field = value
        }

    val commandsToCommit by ctx::commandsToCommit

    val term
        get() = persistentState.currentTerm

    val processId
        get() = ctx.env.processId

    val lastLogId
        get() = ctx.env.storage.readLastLogId()

    val votedFor
        get() = persistentState.votedFor

    fun resetTimeout(timeout: Timeout) = ctx.env.startTimeout(timeout)
    fun send(srcId: Int, msg: Message) = ctx.env.send(srcId, msg)
    fun onClientCommandResult(result: CommandResult) = ctx.env.onClientCommandResult(result)

    abstract fun onTimeout()
    abstract fun onClientCommand(command: Command)

    abstract fun handleAppendEntryResult(srcId: Int, msg: AppendEntryResult)
    abstract fun handleAppendEntryRpc(srcId: Int, msg: AppendEntryRpc)
    abstract fun handleClientCommandResult(srcId: Int, msg: ClientCommandResult)
    abstract fun handleRequestVoteResult(srcId: Int, msg: RequestVoteResult)

    fun handleClientCommandRpc(srcId: Int, msg: ClientCommandRpc) = onClientCommand(msg.command)

    fun handleRequestVoteRpc(srcId: Int, message: RequestVoteRpc) {
        val newerTerm = message.term > term
        val upToDate = message.lastLogId >= lastLogId
        val alreadyVotedFor = votedFor == srcId

        val voteGranted = when {
            newerTerm -> {
                val newLeader = if (upToDate) srcId else null
                transitionTo(Follower(ctx)).apply {
                    persistentState = PersistentState(message.term, newLeader)
                    leaderId = newLeader
                }
                upToDate
            }

            alreadyVotedFor -> true
            else -> false
        }

        send(srcId, RequestVoteResult(max(term, message.term), voteGranted))
    }


    /**
     * Transition to a new state, updating the current state of the Raft node.
     * This method is used to change the node's role in the Raft protocol,
     * such as from Follower to Candidate or Leader.
     */
    fun <T : BaseState> transitionTo(newState: T): T {
        ctx.state = newState
        return newState
    }
}

class RaftNodeContext(
    /**
     * The environment in which the Raft node operates, providing necessary configurations
     * such as process ID, number of processes, and timeout settings.
     */
    val env: Environment
) {

    /**
     * The current role/state of the Raft node: Follower, Candidate, or Leader.
     * This determines the node's behavior in the Raft protocol.
     * Initialized at startup and updated when the node transitions between roles.
     */
    var state: BaseState = Follower(this)

    /**
     * Queue of commands that have been appended to the log
     * but are not yet committed (i.e., not yet applied to the state machine).
     * Once a command’s log entry is safely replicated (i.e., commitIndex is advanced),
     * it will be applied and removed from this queue.
     */
    var commandsToCommit = LinkedList<Command>()

    /**
     * The index of the highest log entry known to be committed.
     * All log entries with index ≤ commitIndex are guaranteed to be safely replicated
     * on a majority of nodes and can be applied to the state machine.
     */
    var commitIndex: Int = 0
}