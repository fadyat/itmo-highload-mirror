package raft

import raft.Message.*
import raft.state.RaftNodeContext

/**
 * Raft algorithm implementation.
 * All functions are called from the single main thread.
 *
 * @author Artyom Fadeyev
 */
class ProcessImpl(private val env: Environment) : Process {
    private val ctx = RaftNodeContext(env)

    override fun onTimeout() = ctx.state.onTimeout()

    override fun onClientCommand(command: Command) = ctx.state.onClientCommand(command)

    override fun onMessage(srcId: Int, message: Message) {
        with(ctx.state) {
            when (message) {
                is AppendEntryResult -> handleAppendEntryResult(srcId, message)
                is AppendEntryRpc -> handleAppendEntryRpc(srcId, message)
                is ClientCommandResult -> handleClientCommandResult(srcId, message)
                is ClientCommandRpc -> handleClientCommandRpc(srcId, message)
                is RequestVoteResult -> handleRequestVoteResult(srcId, message)
                is RequestVoteRpc -> handleRequestVoteRpc(srcId, message)
            }
        }
    }
}
