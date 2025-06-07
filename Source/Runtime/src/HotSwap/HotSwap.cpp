#include "HotSwap/HotSwap.h"
#include "Niflect/Util/SystemUtil.h"
#include "Niflect/Util/StringUtil.h"

bool CHotSwap::CopyPluginFromSourceDirPath() const
{
	const auto srcLibFilePath = ConvertToLibFilePath(m_sourceDirPath, m_moduleName);
	const auto dstLibFilePath = ConvertToLibFilePath(m_pluginDirPath, m_moduleName);
	NiflectUtil::MakeDirectories(dstLibFilePath);
	if (NiflectUtil::CopyFile(srcLibFilePath, dstLibFilePath))
		return true;
	return false;
}