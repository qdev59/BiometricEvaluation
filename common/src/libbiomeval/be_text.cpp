/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <be_text.h>

void
BiometricEvaluation::Text::removeLeadingTrailingWhitespace(string &s)
{
	for (unsigned int idx = 0; idx < s.length(); idx++) {
		if (std::isspace(s[idx])) {
			s.erase(idx, 1);
			idx--;
		} else {
			break;
		}
	}
	for (int idx = s.length() - 1; idx >= 0; idx--) {
		if (std::isspace(s[idx])) {
			s.erase(idx, 1);
		} else {
			break;
		}
	}
}
