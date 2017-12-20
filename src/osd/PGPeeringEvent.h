// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#pragma once

#include <boost/statechart/event.hpp>

#include "osd/osd_types.h"

class MOSDPGLog;

class PGPeeringEvent {
  epoch_t epoch_sent;
  epoch_t epoch_requested;
  boost::intrusive_ptr< const boost::statechart::event_base > evt;
  string desc;
public:
  MEMPOOL_CLASS_HELPERS();
  template <class T>
  PGPeeringEvent(
    epoch_t epoch_sent,
    epoch_t epoch_requested,
    const T &evt_)
    : epoch_sent(epoch_sent),
      epoch_requested(epoch_requested),
      evt(evt_.intrusive_from_this()) {
    stringstream out;
    out << "epoch_sent: " << epoch_sent
	<< " epoch_requested: " << epoch_requested << " ";
    evt_.print(&out);
    desc = out.str();
  }
  epoch_t get_epoch_sent() {
    return epoch_sent;
  }
  epoch_t get_epoch_requested() {
    return epoch_requested;
  }
  const boost::statechart::event_base &get_event() {
    return *evt;
  }
  const string& get_desc() {
    return desc;
  }
};
typedef ceph::shared_ptr<PGPeeringEvent> PGPeeringEventRef;

struct MInfoRec : boost::statechart::event< MInfoRec > {
  pg_shard_t from;
  pg_info_t info;
  epoch_t msg_epoch;
  MInfoRec(pg_shard_t from, const pg_info_t &info, epoch_t msg_epoch) :
    from(from), info(info), msg_epoch(msg_epoch) {}
  void print(std::ostream *out) const {
    *out << "MInfoRec from " << from << " info: " << info;
  }
};

struct MLogRec : boost::statechart::event< MLogRec > {
  pg_shard_t from;
  boost::intrusive_ptr<MOSDPGLog> msg;
  MLogRec(pg_shard_t from, MOSDPGLog *msg) :
    from(from), msg(msg) {}
  void print(std::ostream *out) const {
    *out << "MLogRec from " << from;
  }
};

struct MNotifyRec : boost::statechart::event< MNotifyRec > {
  pg_shard_t from;
  pg_notify_t notify;
  uint64_t features;
  MNotifyRec(pg_shard_t from, const pg_notify_t &notify, uint64_t f) :
    from(from), notify(notify), features(f) {}
  void print(std::ostream *out) const {
    *out << "MNotifyRec from " << from << " notify: " << notify
	 << " features: 0x" << hex << features << dec;
  }
};

struct MQuery : boost::statechart::event< MQuery > {
  pg_shard_t from;
  pg_query_t query;
  epoch_t query_epoch;
  MQuery(pg_shard_t from, const pg_query_t &query, epoch_t query_epoch):
    from(from), query(query), query_epoch(query_epoch) {}
  void print(std::ostream *out) const {
    *out << "MQuery from " << from
	 << " query_epoch " << query_epoch
	 << " query: " << query;
  }
};

struct MTrim : boost::statechart::event<MTrim> {
  epoch_t epoch;
  int from;
  shard_id_t shard;
  eversion_t trim_to;
  MTrim(epoch_t epoch, int from, shard_id_t shard, eversion_t trim_to)
    : epoch(epoch), from(from), shard(shard), trim_to(trim_to) {}
  void print(std::ostream *out) const {
    *out << "MTrim epoch " << epoch << " from " << from << " shard " << shard
	 << " trim_to " << trim_to;
  }
};

struct RequestBackfillPrio : boost::statechart::event< RequestBackfillPrio > {
  unsigned priority;
  explicit RequestBackfillPrio(unsigned prio) :
    boost::statechart::event< RequestBackfillPrio >(),
    priority(prio) {}
  void print(std::ostream *out) const {
    *out << "RequestBackfillPrio: priority " << priority;
  }
};

struct RequestRecoveryPrio : boost::statechart::event< RequestRecoveryPrio > {
  unsigned priority;
  explicit RequestRecoveryPrio(unsigned prio) :
    boost::statechart::event< RequestRecoveryPrio >(),
    priority(prio) {}
  void print(std::ostream *out) const {
    *out << "RequestRecoveryPrio: priority " << priority;
  }
};

#define TrivialEvent(T) struct T : boost::statechart::event< T > { \
    T() : boost::statechart::event< T >() {}			   \
    void print(std::ostream *out) const {			   \
      *out << #T;						   \
    }								   \
  };

TrivialEvent(NullEvt)
TrivialEvent(RemoteBackfillReserved)
TrivialEvent(RemoteReservationRejected)
TrivialEvent(RemoteReservationRevokedTooFull)
TrivialEvent(RemoteReservationRevoked)
TrivialEvent(RemoteReservationCanceled)
TrivialEvent(RemoteRecoveryReserved)
TrivialEvent(RecoveryDone)

struct DeferRecovery : boost::statechart::event<DeferRecovery> {
  float delay;
  explicit DeferRecovery(float delay) : delay(delay) {}
  void print(std::ostream *out) const {
    *out << "DeferRecovery: delay " << delay;
  }
};

struct DeferBackfill : boost::statechart::event<DeferBackfill> {
  float delay;
  explicit DeferBackfill(float delay) : delay(delay) {}
  void print(std::ostream *out) const {
    *out << "DeferBackfill: delay " << delay;
  }
};
