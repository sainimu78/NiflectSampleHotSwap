set(ModuleName Runtime)

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
set(SrcAll "")
list(APPEND SrcAll ${ModuleSources})
list(APPEND SrcAll ${ModuleHeaders})

add_library(${ModuleName} SHARED ${SrcAll})

target_precompile_headers(${ModuleName} PUBLIC ${ModuleIncludePath}/RuntimeCommon.h)

target_include_directories(${ModuleName}
	PRIVATE ${IncludePathsPrivate}
	PUBLIC ${IncludePathsPublic}
)

target_compile_definitions(${ModuleName} PRIVATE -DRUNTIME_EXPORTS)

list(APPEND v_ListAccessorSettingHeaderFilePath ${ModuleIncludePath}/AccessorSetting.h)
#list(APPEND v_ListModuleIncludeDirPath ${IncludePathsPrivate})
#list(APPEND v_ListModuleIncludeDirPath ${IncludePathsPublic})
list(APPEND v_ListModuleHeaderFilePath ${ModuleHeaders})
set(v_EnabledLoadTimeModuleRegistrar TRUE)
#set(v_ModuleAPIMacro ENGINE_API)
#set(v_ModuleAPIMacroHeaderFilePath ${ModuleIncludePath}/Engine/EngineCommon.h)
include(${c_RootProjectDirPath}/Niflect/Default.cmake)

#include(${c_ProjectDirPath}/Install.cmake)