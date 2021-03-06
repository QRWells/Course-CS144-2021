#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : _capacity(capacity) {}

size_t ByteStream::write(const string &data) {
    if (input_ended())
        return 0;

    size_t res = 0;
    for (auto const c : data) {
        if (_buffer_size == _capacity)
            break;
        _buffer.push_front(c);
        ++res;
        ++_buffer_size;
        ++_bytes_written;
    }
    return res;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    auto it = _buffer.crbegin();
    auto const &end = _buffer.crend();
    string res{};
    for (size_t i = 0; i < len && it != end; ++i)
        res += *it++;

    return res;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    if (len >= _buffer_size) {
        if (_buffer.front() == '\0')
            _eof = true;
        _bytes_read += _buffer_size;
        _buffer.clear();
        _buffer_size = 0;
        return;
    }
    for (size_t i = 0; i < len; ++i) {
        if (_buffer.back() == '\0')
            _eof = true;
        else
            --_buffer_size;

        ++_bytes_read;
        _buffer.pop_back();
    }
    if (_buffer.back() == '\0')
        _eof = true;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    if (len == 0)
        return "";
    string res = peek_output(len);
    pop_output(len);
    return res;
}

void ByteStream::end_input() {
    _writable = false;
    _eof = _buffer_size == 0;
    _buffer.push_front('\0');
}

bool ByteStream::input_ended() const { return !_writable; }

size_t ByteStream::buffer_size() const { return _buffer_size; }

bool ByteStream::buffer_empty() const { return _buffer_size == 0; }

bool ByteStream::eof() const { return _eof; }

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read; }

size_t ByteStream::remaining_capacity() const { return _capacity - _buffer_size; }
