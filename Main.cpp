#include <extdll.h>
#include <meta_api.h>
#include "Main.h"
// #include <string>
// #include <algorithm>
// #include <cctype>
// #include <memory>
// #include <vector>
// #include <string>
#include <thread>
#include <fstream>

using namespace std;
using namespace httplib;


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

// size_t GetPluginRelPath(char *path, size_t maxPathLength) {
// 	const char *pluginAbsPath = GET_PLUGIN_PATH(PLID);
// 	const char *gamedirAbsPath = GET_GAME_INFO(PLID, GINFO_GAMEDIR);
// 	const char *pluginRelPath = &pluginAbsPath[0 + 1]; // + slash

// 	// Find last slash
// 	const char *searchChar = pluginRelPath + strlen(pluginRelPath) - 1; // go to string end

// 	while (true) {
// 		if (*searchChar == '/' || *searchChar == '\\') {
// 			break; // ok we found it
// 		}

// 		searchChar--;
// 	}

// 	size_t charsToCopy = Min((size_t)searchChar - (size_t)pluginRelPath + 1, maxPathLength); // with slash
// 	strncpy(path, pluginRelPath, charsToCopy);
// 	path[charsToCopy] = '\0';

// 	return charsToCopy;
// }

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


thread* th;
Server* svr;

void StartSRV(){
    svr = new Server();
	// load config
    
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

    //SERVER_PRINT(g_ExecConfigCmd);

	ifstream file;
	file.open(g_ExecConfigCmd);
    std::string line;
    while (std::getline(file, line)) {		
		svr->set_mount_point("/", line);
    };
	file.close();

    svr->listen("0.0.0.0", 80);
}
void StopSRV(){
	if(svr){
		svr->stop();
	}
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
	StopSRV();
	return TRUE;
}

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs) {
	// pFunctionTable->pfnGetEntityAPI2 = &GetEntityAPI2;
	// pFunctionTable->pfnGetEntityAPI2_Post = &GetEntityAPI2_Post;

	gpMetaGlobals = pMGlobals;
	gpGamedllFuncs = pGamedllFuncs;

	th = new thread(StartSRV);
	th->detach();

	return TRUE;
}

