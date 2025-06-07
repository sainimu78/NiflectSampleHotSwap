#include "HotSwap/HotSwap.h"
#include "Niflect/Util/SystemUtil.h"
#include "Niflect/Util/StringUtil.h"

bool CHotSwap::CopyPluginFromSourceDirPath() const
{
	const auto dstLibFilePath = this->GetDstLibFilePath();
	const auto srcLibFilePath = this->GetSrcLibFilePath();
	NiflectUtil::MakeDirectories(dstLibFilePath);
	if (CopyFile(srcLibFilePath, dstLibFilePath))
		return true;
	ASSERT(false);
	return false;
}
#ifdef WIN32
static Niflect::CString GetSrcPdbFilePath(const Niflect::CString& srcLibFilePath)
{
	return NiflectUtil::RemoveFileExt(srcLibFilePath) + ".pdb";
}
#else
#endif
void CHotSwap::BeforeLoading()
{
#ifdef WIN32
	const auto srcPdbFilePath = GetSrcPdbFilePath(this->GetSrcLibFilePath());
	RenameFile(srcPdbFilePath, srcPdbFilePath + ".bak");
#else
#endif
}
void CHotSwap::AfterLoading()
{
#ifdef WIN32
	const auto srcPdbFilePath = GetSrcPdbFilePath(this->GetSrcLibFilePath());
	RenameFile(srcPdbFilePath + ".bak", srcPdbFilePath);
#else
#endif
}
Niflect::CString CHotSwap::GetVersionedModuleName() const
{
	return m_moduleName;
	//return NiflectUtil::FormatString("%s_V%u", m_moduleName.c_str(), m_runtimeVersion);
}

#include <fstream>
#include <system_error>

#if defined(_WIN32)
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#ifdef WIN32
#undef CopyFile
#endif

bool CopyFile(const Niflect::CString& srcFilePath, const Niflect::CString& dstFilePath) {
#if defined(_WIN32)
	// Windows实现
	return CopyFileA(srcFilePath.c_str(), dstFilePath.c_str(), FALSE) != 0;

#else
	// POSIX系统实现 (Linux/macOS/BSD等)
	int src_fd = -1, dst_fd = -1;
	bool success = false;

	if ((src_fd = open(srcFilePath.c_str(), O_RDONLY)) < 0) return false;

	// 获取源文件权限
	struct stat src_stat;
	if (fstat(src_fd, &src_stat) != 0) {
		close(src_fd);
		return false;
	}

	// 创建目标文件
	if ((dst_fd = open(dstFilePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode)) < 0) {
		close(src_fd);
		return false;
	}

	// 缓冲区复制
	char buffer[65536]; // 64KB缓冲区
	ssize_t bytes_read;
	while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
		if (write(dst_fd, buffer, bytes_read) != bytes_read) {
			break;
		}
	}

	success = (bytes_read == 0);

	close(src_fd);
	close(dst_fd);
	return success;
#endif
}

#if COPY_AND_RETRY_IF_FAILED
#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#endif

#ifdef WIN32
#undef CopyFile
#endif

bool CopyFile(const Niflect::CString& srcFilePath, const Niflect::CString& dstFilePath) {
#if defined(_WIN32)
	// Windows实现
	const int MAX_ATTEMPTS = 5;
	const int DELAY_MS = 100;

	for (int i = 0; i < MAX_ATTEMPTS; ++i) {
		if (CopyFileA(srcFilePath.c_str(), dstFilePath.c_str(), FALSE)) {
			return true;
		}

		DWORD error = GetLastError();
		if (error == ERROR_SHARING_VIOLATION || error == ERROR_ACCESS_DENIED) {
			Sleep(DELAY_MS);
			continue;
		}

		return false;
	}
	return false;

#elif defined(__linux__)
	// Linux实现（使用sendfile系统调用）
	int src_fd = -1, dst_fd = -1;
	bool result = false;
	struct stat stat_buf;

	do {
		if ((src_fd = open(srcFilePath.c_str(), O_RDONLY)) == -1) break;
		if (fstat(src_fd, &stat_buf) == -1) break;
		if ((dst_fd = open(destination.c_str(), O_WRONLY | O_CREAT | O_TRUNC, stat_buf.st_mode)) == -1) break;

		off_t offset = 0;
		ssize_t sent;
		while (offset < stat_buf.st_size) {
			if ((sent = sendfile(dst_fd, src_fd, &offset, stat_buf.st_size - offset)) == -1) {
				if (errno == EINTR) continue;
				break;
			}
			offset += sent;
		}
		result = (offset == stat_buf.st_size);
	} while (false);

	if (src_fd != -1) close(src_fd);
	if (dst_fd != -1) close(dst_fd);
	return result;

#elif defined(__APPLE__)
	// macOS实现（使用fcopyfile API）
	int src_fd = -1, dst_fd = -1;
	bool result = false;

	do {
		if ((src_fd = open(srcFilePath.c_str(), O_RDONLY)) == -1) break;
		if ((dst_fd = open(dstFilePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) break;

		// 使用fcopyfile复制数据和元数据
		result = (fcopyfile(src_fd, dst_fd, nullptr, COPYFILE_ALL) == 0);
	} while (false);

	if (src_fd != -1) close(src_fd);
	if (dst_fd != -1) close(dst_fd);
	return result;

#else
	// 通用实现（适用于其他POSIX系统）
	std::ifstream src(srcFilePath, std::ios::binary);
	if (!src.is_open()) return false;

	std::ofstream dst(dstFilePath, std::ios::binary | std::ios::trunc);
	if (!dst.is_open()) return false;

	dst << src.rdbuf();
	return src.good() && dst.good();
#endif
}
#endif

#include <string>
#include <system_error>

#if defined(_WIN32)
#include <windows.h>
#else
#include <cstdio>
#include <cerrno>
#endif

bool RenameFile(const Niflect::CString& old_path, const Niflect::CString& new_path) {
#if defined(_WIN32)
	// Windows实现
	return MoveFileExA(old_path.c_str(), new_path.c_str(),
		MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
	// POSIX实现 (Linux/macOS/BSD等)
	return std::rename(old_path.c_str(), new_path.c_str()) == 0;
#endif
}