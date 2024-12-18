APPLICATION = awtrix

BOARD ?= esp32-wroom-32

RIOTBASE ?= $(CURDIR)/../../../RIOT

WIFI_SSID ?= "AllRIOT"
WIFI_PASS ?= "JointheRIOT"

QUIET ?= 1

CFLAGS += -DDHT_PARAM_PIN=21

USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += netdev_default
USEMODULE += sock_dns
USEMODULE += auto_init_sock_dns

USEPKG += paho-mqtt

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_icmpv6_error
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_netif_bus

USEMODULE += sock_async_event
USEMODULE += sock_ip
USEMODULE += sock_udp
USEMODULE += sock_tcp
USEMODULE += esp_wifi
USEMODULE += posix_sleep

USEMODULE += ztimer
USEMODULE += ztimer_msec

USEMODULE += saul_default
USEMODULE += dht # requires driver for dht sensor

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter arch_esp,$(FEATURES_USED)))
  CFLAGS += -DWIFI_SSID=\"$(WIFI_SSID)\"
  CFLAGS += -DWIFI_PASS=\"$(WIFI_PASS)\"
endif
