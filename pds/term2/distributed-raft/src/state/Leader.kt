package raft.state

import raft.*
import raft.Message.*

class Leader(ctx: RaftNodeContext) : BaseState(ctx) {

    override fun onTimeout() {
        TODO("Not yet implemented")
    }

    override fun onClientCommand(command: Command) {
        TODO("Not yet implemented")
    }

    override fun handleAppendEntryResult(srcId: Int, msg: AppendEntryResult) {
        TODO("Not yet implemented")
    }

    override fun handleAppendEntryRpc(srcId: Int, msg: AppendEntryRpc) {
        TODO("Not yet implemented")
    }

    override fun handleClientCommandResult(srcId: Int, msg: ClientCommandResult) {
        TODO("Not yet implemented")
    }

    override fun handleRequestVoteResult(srcId: Int, msg: RequestVoteResult) = Unit
}