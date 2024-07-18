#include <extdll.h>
#include <meta_api.h>
#include "Main.h"

#include <thread>
#include <fstream>

enginefuncs_t g_engfuncs;
globalvars_t *gpGlobals;

C_DLLEXPORT
#ifdef _WIN32
__declspec(naked)
#endif
void GiveFnptrsToDll(enginefuncs_t *pEngFuncs, globalvars_t *pGlobalVars) {
#ifdef _WIN32
	__asm
	{
		push ebp
		mov  ebp, esp
		sub  esp, __LOCAL_SIZE
		push ebx
		push esi
		push edi
	}
#endif

	memcpy(&g_engfuncs, pEngFuncs, sizeof(g_engfuncs));
	gpGlobals = pGlobalVars;

#ifdef _WIN32
	__asm
	{
		pop edi
		pop esi
		pop ebx
		mov esp, ebp
		pop ebp
		ret 8
	}
#endif
}


template <typename T>
T Min(T a, T b) {
	return (a < b) ? a : b;
}

void NormalizePath(char *path)
{
	for (char *cp = path; *cp; cp++) {
		if (isupper(*cp))
			*cp = tolower(*cp);

		if (*cp == '\\')
			*cp = '/';
	}
}




plugin_info_t Plugin_info = {
	META_INTERFACE_VERSION, // ifvers
	"httpd",      // name
	CPPHTTPLIB_VERSION,         // version
	"2023.01.01",           // date
	"Aly4",     // author
	"https://github.com/yhirose/cpp-httplib", // url
	"HTTPD",                 // logtag, all caps please
	PT_ANYTIME,             // (when) loadable
	PT_ANYTIME,             // (when) unloadable
};

meta_globals_t *gpMetaGlobals;
gamedll_funcs_t *gpGamedllFuncs;
mutil_funcs_t *gpMetaUtilFuncs;

C_DLLEXPORT int Meta_Query(char *pchInterfaceVersion, plugin_info_t **pPluginInfo, mutil_funcs_t *pMetaUtilFuncs) {
	*pPluginInfo = &Plugin_info;
	gpMetaUtilFuncs = pMetaUtilFuncs;

	return TRUE;
}


std::thread* th;
httplib::Server* svr;

void DEBUG_PRNT( const char *fmt, ...){
	char logstring[2048];

	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(logstring, sizeof(logstring), fmt, argptr);
	va_end(argptr);

	SERVER_PRINT(logstring);
}

void Listen(){
    svr = new httplib::Server();
    
	char g_ExecConfigCmd[PATH_MAX];
	const char *pszGameDir = GET_GAME_INFO(PLID, GINFO_GAMEDIR);
	const char *pszPluginDir = GET_PLUGIN_PATH(PLID);

	char szRelativePath[PATH_MAX];
	strncpy(szRelativePath, &pszPluginDir[strlen(pszGameDir) + 1], sizeof(szRelativePath) - 1);
	szRelativePath[sizeof(szRelativePath) - 1] = '\0';
	NormalizePath(szRelativePath);

	char *pos = strrchr(szRelativePath, '/');
	if (pos) {
		*(pos + 1) = '\0';
	}

	snprintf(g_ExecConfigCmd, sizeof(g_ExecConfigCmd), "cstrike/%s%s.cfg", szRelativePath, Plugin_info.name);
	DEBUG_PRNT("HTTPD: config file path : %s\n",g_ExecConfigCmd);

	std::ifstream file(g_ExecConfigCmd);
	if (file.is_open()) {
    	std::string line;
    	while (std::getline(file, line)) {
			DEBUG_PRNT("HTTPD: load config line: %s\n",line.c_str());
			svr->set_mount_point("/", line.c_str());
    	}
    	file.close();
	}
	svr->listen("0.0.0.0", 80);
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
	if(svr){
		svr->stop();
	}
	return TRUE;
}

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs) {
	gpMetaGlobals = pMGlobals;
	gpGamedllFuncs = pGamedllFuncs;
	th = new std::thread(Listen);
	th->detach();
	return TRUE;
}

