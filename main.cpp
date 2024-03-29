#include "SKSE/API.h"
#include "SKSE/Impl/Stubs.h"
#include "Windows.h"
#include "external/higgsinterface001.h"
#include "main_plugin.h"  // main source for plugin game logic

#include <spdlog/sinks/basic_file_sink.h>

void MessageListener(SKSE::MessagingInterface::Message* message);
void OnPapyrusVRMessage(SKSE::MessagingInterface::Message* message);
void SetupLog();

static SKSE::detail::SKSEMessagingInterface* g_messaging;
static SKSE::PluginHandle                    g_pluginHandle = 0xFFFFFFFF;
PapyrusVRAPI*                                g_papyrusvr;

void InitializeHooking()
{
	auto& trampoline = SKSE::GetTrampoline();
	trampoline.create(64);
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
	SKSE::Init(skse);
	SetupLog();

	SKSE::GetMessagingInterface()->RegisterListener(MessageListener);

	g_pluginHandle = skse->GetPluginHandle();
	g_messaging = (SKSE::detail::SKSEMessagingInterface*)skse->QueryInterface(
		SKSE::LoadInterface::kMessaging);

	return true;
}

// Receives messages about the game's state that SKSE broadcasts to all plugins.
void MessageListener(SKSE::MessagingInterface::Message* message)
{
	using namespace SKSE::log;

	switch (message->type)
	{
	case SKSE::MessagingInterface::kPostLoad:
		// trace("kPostLoad: sent to registered plugins once all plugins have been loaded");
		info("Registering for SkyrimVRTools messages");
		g_messaging->RegisterListener(g_pluginHandle, "SkyrimVRTools", OnPapyrusVRMessage);
		break;

	case SKSE::MessagingInterface::kPostPostLoad:
		info("kPostPostLoad: querying higgs interface");
		g_higgsInterface = HiggsPluginAPI::GetHiggsInterface001(g_pluginHandle, g_messaging);
		if (g_higgsInterface) { info("Got higgs interface"); }
		else { info("HIGGS interface not found"); }

		info("kPostPostLoad: querying VRIK interface");
		g_vrikInterface = vrikPluginApi::getVrikInterface001(g_pluginHandle, g_messaging);
		if (g_vrikInterface) { info("Got VRIK interface"); }
		else { info("VRIK interface not found"); }

		break;

	case SKSE::MessagingInterface::kDataLoaded:
		InitializeHooking();
		griptweaks::StartMod();
		break;

	default:
		break;
	}
}

// Listener for papyrusvr Messages
void OnPapyrusVRMessage(SKSE::MessagingInterface::Message* message)
{
	if (message)
	{
		if (message->type == kPapyrusVR_Message_Init && message->data)
		{
			SKSE::log::info(
				"SkyrimVRTools Init Message recived with valid data, registering for "
				"callback");
			g_papyrusvr = (PapyrusVRAPI*)message->data;

			griptweaks::g_OVRHookManager = g_papyrusvr->GetOpenVRHook();
		}
	}
}

// Initialize logging system.
void SetupLog()
{
	auto logsFolder = SKSE::log::log_directory();
	if (!logsFolder)
	{
		SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
		return;
	}
	auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
	auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
	auto fileLoggerPtr =
		std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
	auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
	spdlog::set_default_logger(std::move(loggerPtr));
	spdlog::set_level(spdlog::level::trace);
	spdlog::flush_on(spdlog::level::trace);
}
