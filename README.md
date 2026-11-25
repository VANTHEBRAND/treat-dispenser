## ESP32 Twitch treat dispenser

Install the Arduino ESP32 core and the Links2004 WebSockets library. Keep the Wi-Fi and Twitch credentials in the ignored `secret.h`. If a previous device sent its OAuth credential over the original plaintext connection, revoke that authorization in Twitch and provision a new chat token before flashing.

Define `SSID`, `PASS`, `TWITCH_OAUTH_TOKEN` (including its `oauth:` prefix), `TWITCH_NICK`, and `TWITCH_CHANNEL` (including the leading `#`) as string literals. Set `TREATMASTER_ADMIN_ID` and `TREATMASTER_ID` to quoted numeric Twitch user IDs. You can use the same ID for both. The allowlist is configured at flash time.

Define `TWITCH_ROOT_CA` as a PEM string containing a trusted root CA for the certificate chain of `irc-ws.chat.twitch.tv`, obtained from the CA's official repository. Keep this trust anchor current when rebuilding firmware. The ESP32 synchronizes its clock, authenticates the Twitch server over TLS, and accepts the exact `!treat` command from either configured user in the configured channel.
