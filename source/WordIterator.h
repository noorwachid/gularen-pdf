#include <string_view>

using std::string_view;

class WordIterator {
public:
	WordIterator(string_view line): _line(line) {
		_index = 0;
		next();
	}

	bool hasNext() {
		return _startIndex < _index;
	}

	void next() {
		while (_inBound() && _line[_index] == ' ') {
			_index += 1;
		}
		
		_startIndex = _index;

		while (_inBound() && _line[_index] != ' ') {
			_index += 1;
		}
	}

	string_view get() {
		return _line.substr(_startIndex, _index - _startIndex);
	}

private:
	bool _inBound() {
		return _index < _line.size();
	}


private:
	size_t _startIndex;
	size_t _index;
	string_view _line;
};

