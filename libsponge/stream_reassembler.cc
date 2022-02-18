#include "stream_reassembler.hh"

#include <cstddef>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _buffer(capacity, 0), _data_available(capacity, false), _end(capacity), _output(capacity), _capacity(capacity) {}

void StreamReassembler::write_stream() {
    string ss;
    while (_data_available.front()) {
        ss += _buffer.front();
        _buffer.pop_front();
        _data_available.pop_front();
        _buffer.emplace_back(0);
        _data_available.emplace_back(false);
    }
    if (!ss.empty()) {
        _output.write(ss);
        _last_unassembled += ss.size();
        _unassembled_bytes -= ss.size();
    }
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    _eof = eof ? eof : _eof;

    auto const len = data.size();

    if (len == 0 && _eof && _unassembled_bytes == 0) {
        _output.end_input();
        return;
    }

    if (index >= _last_unassembled + _capacity)
        return;

    auto const offset = abs(static_cast<int64_t>(index) - static_cast<int64_t>(_last_unassembled));

    if (index >= _last_unassembled) {
        auto const real_len = min(len, _capacity - _output.buffer_size() - offset);
        if (real_len < len)
            _eof = false;

        for (size_t i = 0; i < real_len; i++) {
            if (_data_available[i + offset])
                continue;
            _buffer[i + offset] = data[i];
            _data_available[i + offset] = true;
            _unassembled_bytes++;
        }
    } else if (index + len > _last_unassembled) {
        auto const real_len = min(len - offset, _capacity - _output.buffer_size());
        if (real_len < len - offset)
            _eof = false;

        for (size_t i = 0; i < real_len; i++) {
            if (_data_available[i])
                continue;
            _buffer[i] = data[i + offset];
            _data_available[i] = true;
            _unassembled_bytes++;
        }
    }

    write_stream();
    if (_eof && _unassembled_bytes == 0)
        _output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_bytes; }

bool StreamReassembler::empty() const { return _unassembled_bytes == 0; }
