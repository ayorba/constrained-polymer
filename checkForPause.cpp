#include <iostream>
#include <iomanip>
#include <string>
#include <termios.h>
#include <sys/select.h>
#include <unistd.h>
#include "MD.h"


// Poll stdin without blocking. Temporarily disables canonical mode so a single
// 'p' keypress (no Enter required) triggers a pause. Returns true if a pause
// occurred so the caller can reset its display state.


bool MD::checkForPause(int step)
{
	if (!isatty(STDIN_FILENO))
		return false;

	// Disable canonical mode and echo just long enough to poll for one byte.
	struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	newt.c_cc[VMIN]  = 0;
	newt.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	struct timeval tv = {0, 0};
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);

	bool triggered = false;
	if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0)
	{
		char c;
		if (read(STDIN_FILENO, &c, 1) == 1 && (c == 'p' || c == 'P'))
			triggered = true;
	}

	// Restore terminal before printing anything.
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	if (!triggered)
		return false;

	// Move past both display lines (status + hint) before printing.
	std::cout << "\n\n";

	// Save current state.
	writeFiles(step);

	// Print resume command.
	std::cout << "--- PAUSED at step " << step << " ---\n";
	std::cout << "Configuration saved to: " << OUT << "config_file.txt\n\n";
	std::cout << "Resume command:\n  ./polymer "
	          << simtype << " "
	          << std::scientific << std::setprecision(6)
	          << deltat     << " "
	          << damping    << " "
	          << temperature << " "
	          << std::defaultfloat << writestep << " "
	          << OUT << " config_file.txt "
	          << OUT << " "
	          << std::scientific << std::setprecision(6) << CF_mag << " "
	          << "1\n";

	std::cout << "\nPress Enter to continue..." << std::flush;
	std::string dummy;
	std::getline(std::cin, dummy);
	std::cout << "\n";

	return true;
}
