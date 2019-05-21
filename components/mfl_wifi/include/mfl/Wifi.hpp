#pragma once

#include <lwip/ip_addr.h>
#include <esp_event.h>
#include <esp_err.h>

#include <map>
#include <functional>
#include <vector>
#include <optional>

namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

struct Wifi {
	using IpHandler =  std::function<void(const ip4_addr&)>;
	using FailHandler = std::function<void()>;
    using Bssid = std::vector<uint8_t>;

	enum class Protocol {
		tcp,
		udp,
	};

    // FIXME: replace std::vector with std::optional<std::array<uint8_t, 6>> as soon as the toolchain supports it
	Wifi(const std::string& hostname, const std::string& ssid, const std::string& key, const Bssid& bssid = Bssid());

	esp_err_t addService(const std::string& type, Protocol protocol, uint16_t port,
			const std::string& name = "",
			const std::map<std::string, std::string>& props = {});

	esp_err_t start(const IpHandler& onIp, const FailHandler& onFail = FailHandler());

private:
	static esp_err_t eventHandler(void *ctx, system_event_t *event);
	static Wifi* activeWifi;

	IpHandler onIp_;
	FailHandler onFail_;

	const std::string hostname_;
	const std::string ssid_;
	const std::string key_;
    const Bssid bssid_;
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl

