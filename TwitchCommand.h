#pragma once

#include <string>

inline bool shouldDispense(const std::string &line, const char *channel,
                           const char *adminId, const char *treatmasterId) {
  if (line.empty() || line.front() != '@') return false;
  const auto tagsEnd = line.find(' ');
  if (tagsEnd == std::string::npos || line.compare(tagsEnd, 2, " :") != 0) return false;
  const auto prefixEnd = line.find(' ', tagsEnd + 2);
  if (prefixEnd == std::string::npos ||
      line.substr(prefixEnd + 1) != std::string("PRIVMSG ") + channel + " :!treat") return false;

  // Twitch supplies user-id in the IRCv3 tags, before the sender prefix and message.
  const std::string tags = line.substr(1, tagsEnd - 1);
  std::string userId;
  size_t start = 0;
  while (start < tags.size()) {
    const auto end = tags.find(';', start);
    const auto tag = tags.substr(start, end - start);
    if (tag.compare(0, 8, "user-id=") == 0) {
      if (!userId.empty()) return false;
      userId = tag.substr(8);
      if (userId.empty() || userId.find_first_not_of("0123456789") != std::string::npos) return false;
    }
    if (end == std::string::npos) break;
    start = end + 1;
  }
  return !userId.empty() && (userId == adminId || userId == treatmasterId);
}
