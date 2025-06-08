#include "HotSwap/HotSwap.h"
#include "Niflect/Util/SystemUtil.h"
#include "Niflect/Util/StringUtil.h"

CHotSwap::~CHotSwap()
{
	this->DestroyInstance();
	NiflectUtil::DeleteDirectory(m_pluginDirPath);
}
Niflect::CString CHotSwap::GetVersionedModuleName() const
{
#ifdef WIN32
	return m_moduleName;
#else
	return NiflectUtil::FormatString("%s_V%u", m_moduleName.c_str(), m_runtimeVersion);
#endif
}
Niflect::CString CHotSwap::GetDesiredPluginLibFilePath() const
{
	return ConvertToLibFilePath(m_pluginDirPath, this->GetVersionedModuleName());
}
bool CHotSwap::CopyPluginFromSourceDirPath() const
{
	const auto srcLibFilePath = ConvertToLibFilePath(m_sourceDirPath, m_moduleName);
	const auto dstLibFilePath = this->GetDesiredPluginLibFilePath();
	NiflectUtil::MakeDirectories(dstLibFilePath);
	if (NiflectUtil::CopyFile(srcLibFilePath, dstLibFilePath))
		return true;
	return false;
}
void CHotSwap::DestroyInstance()
{
	if (m_swappableInstance != NULL)
	{
		m_swappableType = NULL;
		m_swappableInstance = NULL;
	}
	m_module.Unload();
#ifdef WIN32
#else
	NiflectUtil::DeleteFile(this->GetDesiredPluginLibFilePath());
#endif
}