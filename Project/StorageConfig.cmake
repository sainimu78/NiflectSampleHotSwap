set(c_StorageHost "192.168.31.233")

set(c_StorageAddrPath "http://WishingContributor:1@${c_StorageHost}/sainimu78_Storage")
if(WIN32)
	set(c_StorageDirPath "F:/sainimu78_Storage")
else()
	set(c_StorageDirPath "/mnt/Ubuntu_Storage")
endif()

#set(c_VpnPort 1080)
