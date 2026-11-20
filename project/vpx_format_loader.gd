@tool
class_name VpxFormatLoader
extends ResourceFormatLoader
## Format loader class to add WEBM file support.
##
## This class allows WEBM files to be natively shown and loaded through the File System.


const _RESOURCE_NAME: StringName = "VideoStreamVpx"


func _get_recognized_extensions() -> PackedStringArray:
	return PackedStringArray(["webm"])


func _get_resource_type(path: String) -> String:
	if path.get_extension() in _get_recognized_extensions():
		return _RESOURCE_NAME
	
	return ""


func _handles_type(type: StringName) -> bool:
	return type == _RESOURCE_NAME or ClassDB.is_parent_class(_RESOURCE_NAME, type)


func _load(path: String, original_path: String, use_sub_threads: bool, cache_mode: int) -> Variant:
	var video_file: FileAccess = FileAccess.open(path, FileAccess.READ)
	if not video_file:
		return null
	
	var video: VideoStreamVpx = VideoStreamVpx.new()
	video.file = path
	
	return video
