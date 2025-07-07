package raft.state

import raft.*
import raft.Message.*

class Follower(ctx: RaftNodeContext) : BaseState(ctx) {
    var leaderId: Int? = null

    init {
        resetTimeout(Timeout.ELECTION_TIMEOUT)
    }

    private fun forwardClientCommandsToLeader() {
        while (commandsToCommit.isNotEmpty()) {
            val command = commandsToCommit.removeFirst()
            send(leaderId!!, ClientCommandRpc(term, command))
        }
    }

    override fun onTimeout() {
        transitionTo(Candidate(ctx))
    }

    override fun onClientCommand(command: Command) {
        if (leaderId != null) {
            send(leaderId!!, ClientCommandRpc(term, command))
        } else {
            commandsToCommit.add(command)
        }
    }

    override fun handleAppendEntryRpc(srcId: Int, msg: AppendEntryRpc) {
        TODO("Not yet implemented")
    }

    override fun handleClientCommandResult(srcId: Int, msg: ClientCommandResult) {
        onClientCommandResult(msg.result)

        if (leaderId == null && msg.term != term) {
            transitionTo(Follower(ctx)).apply {
                leaderId = srcId
                persistentState = PersistentState(msg.term, null)

                forwardClientCommandsToLeader()
            }
        }
    }

    override fun handleAppendEntryResult(srcId: Int, msg: AppendEntryResult) = Unit
    override fun handleRequestVoteResult(srcId: Int, msg: RequestVoteResult) = Unit
}