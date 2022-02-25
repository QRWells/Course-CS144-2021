#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    auto const &header = seg.header();
    auto seqno = unwrap(header.seqno, _isn, _checkpoint);

    if (header.syn) {  // set parameters if it's SYN
        if (_syn_rec)  // pass if received SYN again
            return;
        _syn_rec = true;
        _isn = header.seqno;
        seqno = 1;
    }

    if (!_syn_rec)  // connection not established
        return;

    if (_fin_rec and header.fin)  // connection finish
        return;

    _fin_rec = header.fin ? header.fin : _fin_rec;  // set if received the finish flag
    auto const idx = seqno - 1;                     // index of the payload

    if (idx >= _reassembler.last_unassembled() + _capacity)  // overflow
        return;

    if (idx + seg.length_in_sequence_space() <= _reassembler.last_unassembled())  // acknowledged
        return;

    _reassembler.push_substring(string(seg.payload().str()), idx, header.fin);
    _checkpoint = seqno;
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    auto const offset = _fin_rec && _reassembler.unassembled_bytes() == 0 ? 2 : 1;
    if (_syn_rec)
        return wrap(_reassembler.last_unassembled() + offset, _isn);
    return {};
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }
