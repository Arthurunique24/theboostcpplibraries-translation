#include <iostream>
#include <fstream>
class CommentsDeleter {
public:
	CommentsDeleter() = delete;
	CommentsDeleter(std::istream &_in, std::ostream &_out) 
		: in(_in), out(_out)
	{ }
	void deleteComments() 
	{
		char c;
		while (in.get(c)) {
			switch (c) {
			case '\'':
			case '\"':
				out << c;
				handleQuote(c);
				break;
			case '\/':
				handleSlash();
				break;
			default:
				out << c;
			}
		}
	}
private:
	void handleQuote(char quote) 
	{
		char c;
		while (in.get(c)) {
			out << c;
			if (c == '\\') {
				in.get(c);
				out << c;
			}
			else if (c == quote) {
				break;
			}
		}
	}
	void handleSlash() 
	{
		char c;
		in.get(c);
		switch (c) {
		case '\/':
			handleDoubleSlash();
			break;
		case '\*':
			handleSlashStar();
			break;
		default:
			out << '\\' << c;
		}
	}
	void handleDoubleSlash() 
	{
		char c;
		while (in.get(c)) {
			if (c == '\\') {
				in.get(c);
			}
			else if (c == '\n') {
				out << c;
				break;
			}
		}
	}
	void handleSlashStar()
	{
		char c;
		while (in.get(c)) {
			if (c == '\\') {
				in.get(c);
			}
			else if (c == '\*') {
				in.get(c);
				if (c == '\/') {
					break;
				}
			}
		}
	}
	std::istream &in;
	std::ostream &out;
};

int main() {
	std::ifstream in("input.txt");
	std::ofstream out("output.txt");
	CommentsDeleter d(in, out);
	d.deleteComments();
}
