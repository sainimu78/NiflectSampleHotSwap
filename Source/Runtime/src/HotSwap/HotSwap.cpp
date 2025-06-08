#include "HotSwap/HotSwap.h"
#include "Niflect/Util/SystemUtil.h"
#include "Niflect/Util/StringUtil.h"

CHotSwap::~CHotSwap()
{
	this->Destroy();
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
bool CHotSwap::CopyPluginFromSourceDirPath() const
{
	const auto srcLibFilePath = ConvertToLibFilePath(m_sourceDirPath, m_moduleName);
	const auto dstLibFilePath = ConvertToLibFilePath(m_pluginDirPath, this->GetVersionedModuleName());
	NiflectUtil::MakeDirectories(dstLibFilePath);
	if (NiflectUtil::CopyFile(srcLibFilePath, dstLibFilePath))
		return true;
	return false;
}