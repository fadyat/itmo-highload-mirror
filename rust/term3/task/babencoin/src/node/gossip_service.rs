use crate::{
    block_forest::BlockForest,
    data::{BlockHash, TransactionHash, VerifiedBlock, VerifiedPeerMessage, VerifiedTransaction},
    node::mining_service::MiningInfo,
    node::peer_service::{PeerCommand, PeerCommandKind, PeerEvent, PeerEventKind, SessionId},
};

use anyhow::{Context, Result};
use futures::{Stream, StreamExt, future::pending, stream};
use log::*;
use rand::seq::SliceRandom;
use serde::{Deserialize, Serialize};
use tokio::{
    select,
    sync::mpsc::{Receiver, Sender},
};

use std::{
    collections::{HashMap, HashSet},
    pin::pin,
    time::Duration,
};

////////////////////////////////////////////////////////////////////////////////

#[derive(Default, Serialize, Deserialize)]
pub struct GossipServiceConfig {
    #[serde(with = "humantime_serde")]
    pub eager_requests_interval: Duration,
}

pub struct GossipService {
    // TODO: your code here.
}

impl GossipService {
    pub fn new(
        config: GossipServiceConfig,
        event_receiver: Receiver<PeerEvent>,
        command_sender: Sender<PeerCommand>,
        block_receiver: Receiver<VerifiedBlock>,
        mining_info_sender: Sender<MiningInfo>,
    ) -> Self {
        // TODO: your code here.
        unimplemented!()
    }

    pub async fn run(&mut self) -> Result<()> {
        // TODO: your code here.
        unimplemented!()
    }

    // TODO: your code here.
}
