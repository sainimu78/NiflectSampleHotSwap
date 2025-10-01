cmake_minimum_required(VERSION 3.10.0)

include(../InitRootPath.cmake)

set(c_ProjectName App)

include(${c_RootCMakeProjectFrameworkDirPath}/ProjectBegin.cmake)
project(${c_ProjectName})
include(${c_RootCMakeProjectFrameworkDirPath}/ProjectEnd.cmake)

include(${c_RootCMakeProjectFrameworkDirPath}/InitCMakeTargetOutputDirPaths.cmake)

cpf_include_install(${c_RootProjectDirPath}/Runtime/Shared.cmake)

set(ModuleName ${c_ProjectName})

set(ModuleRootPath ${c_RootSourceDirPath}/${ModuleName})
set(ModuleSourcePath ${ModuleRootPath}/src)
set(ModuleIncludePath ${ModuleRootPath}/include)

set(IncludePathsPrivate "")
set(IncludePathsPublic "")
list(APPEND IncludePathsPrivate ${ModuleSourcePath})
list(APPEND IncludePathsPublic ${ModuleIncludePath})

file(GLOB_RECURSE ModuleSources ${ModuleSourcePath}/*.cpp ${ModuleSourcePath}/*.h)
create_source_group(${ModuleSourcePath} ${ModuleSources})
file(GLOB_RECURSE ModuleHeaders ${ModuleIncludePath}/*.h)
create_source_group(${ModuleIncludePath} ${ModuleHeaders})

set(AntiCheatIncludePath ${c_RootSourceDirPath}/Plugin/AntiCheat/include)
file(GLOB_RECURSE AntiCheatHeaders ${AntiCheatIncludePath}/*.h)
create_source_group(${AntiCheatIncludePath} ${AntiCheatHeaders})
list(APPEND ModuleHeaders ${AntiCheatHeaders})
list(APPEND IncludePathsPrivate ${AntiCheatIncludePath})

set(SrcAll "")
list(APPEND SrcAll ${ModuleSources})
list(APPEND SrcAll ${ModuleHeaders})

add_executable(${ModuleName} ${SrcAll})

target_include_directories(${ModuleName}
	PRIVATE ${IncludePathsPrivate}
	PUBLIC ${IncludePathsPublic}
)

ngt_target_link_libraries(${ModuleName} PRIVATE Runtime)

list(APPEND v_ListModuleHeaderFilePath ${ModuleHeaders})
set(v_EnabledLoadTimeModuleRegistrar TRUE)
include(${c_RootProjectDirPath}/Niflect/Default.cmake)

include(${c_ProjectDirPath}/Install.cmake)

if(MSVC)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${ModuleName})
	set_target_properties(${ModuleName} PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "$<IF:$<CONFIG:Debug>,${c_ProjectBinDirPathDebug},${c_ProjectBinDirPathRelease}>"
	)
else()
	#target_link_libraries(${ModuleName} PRIVATE pthread)#pthread_create, 使用高版本如 Ubuntu 24 中的标准库可不链接 pthread
	target_link_libraries(${ModuleName} PRIVATE dl) #dlopen, dlclose, dlerror, dlsym, 使用高版本如 Ubuntu 24 中的标准库可不链接 dl
endif()

include(${c_RootCMakeProjectFrameworkDirPath}/Release.cmake)
include(${c_RootCMakeProjectFrameworkDirPath}/Cleanup.cmake)