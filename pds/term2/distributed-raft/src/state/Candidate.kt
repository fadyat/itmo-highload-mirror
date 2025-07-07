package raft.state

import raft.*
import raft.Message.*

class Candidate(ctx: RaftNodeContext) : BaseState(ctx) {
    private val votesReceived = mutableSetOf<Int>()
    private val elected
        get() = votesReceived.size > ctx.env.nProcesses / 2

    init {
        startElection()
    }

    /**
     * The candidate state is responsible for initiating an election.
     * It sends RequestVoteRpc messages to all other processes to gather votes.
     * If it receives a majority of votes, it transitions to the Leader state.
     */
    private fun startElection() {
        persistentState = PersistentState(term + 1, processId)
        votesReceived.add(processId)

        for (i in 1..ctx.env.nProcesses) {
            if (i != processId) {
                send(i, RequestVoteRpc(persistentState.currentTerm, lastLogId))
            }
        }

        resetTimeout(Timeout.ELECTION_TIMEOUT)
    }

    override fun onTimeout() {
        transitionTo(Candidate(ctx))
    }

    override fun handleRequestVoteResult(srcId: Int, msg: RequestVoteResult) {
        if (msg.voteGranted) {
            votesReceived.add(srcId)
            if (elected) {
                transitionTo(Leader(ctx))
            }
        } else if (msg.term > term) {
            transitionTo(Follower(ctx))
        }
    }

    override fun onClientCommand(command: Command) {
        commandsToCommit.add(command)
    }

    override fun handleAppendEntryRpc(srcId: Int, msg: AppendEntryRpc) {
        if (msg.term >= term && msg.prevLogId >= lastLogId) {
            transitionTo(Follower(ctx)).apply {
                leaderId = srcId
                handleAppendEntryRpc(srcId, msg)
            }
        } else {
            send(srcId, AppendEntryResult(term, null))
        }
    }

    override fun handleClientCommandResult(srcId: Int, msg: ClientCommandResult) {
        onClientCommandResult(msg.result)
    }

    override fun handleAppendEntryResult(srcId: Int, msg: AppendEntryResult) = Unit
}