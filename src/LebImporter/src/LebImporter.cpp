

#include "LeCore/LeCore.h"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>


#include "LebImporter/LebImporter.h"

#define LE_CHECK_CGLTF_RESULT(x) if (x != cgltf_result_success) \
								 { \
									LE_ERROR("cgltf function returned an unexpected result. Result value %s", cgltf_result_to_cstr(x));\
								 }

static const char* cgltf_result_to_cstr(cgltf_result res)
{
	switch (res)
	{
	case cgltf_result_success:
		return "cgltf_result_success";
	case cgltf_result_data_too_short:
		return "cgltf_result_success";
	case cgltf_result_unknown_format:
		return "cgltf_result_success";
	case cgltf_result_invalid_json:
		return "cgltf_result_success";
	case cgltf_result_invalid_gltf:
		return "cgltf_result_success";
	case cgltf_result_invalid_options:
		return "cgltf_result_success";
	case cgltf_result_file_not_found:
		return "cgltf_result_success";
	case cgltf_result_io_error:
		return "cgltf_result_success";
	case cgltf_result_out_of_memory:
		return "cgltf_result_success";
	case cgltf_result_legacy_gltf:
		return "cgltf_result_success";
	case cgltf_result_max_enum:
	default:
		LE_ERROR("Unknown cgltf_result enum value!");
		return "cgltf_result_max_enum";
	}
}

LeScene* LebImporter::ImportGltf(const LeChar* pFilename)
{
	cgltf_result res;
	cgltf_options Options =
	{
		.type = cgltf_file_type_gltf
	};
	LeScene* pScene = NULL;

	cgltf_data* pData = NULL;
	res = cgltf_parse_file(&Options, pFilename, &pData);
	LE_CHECK_CGLTF_RESULT(res);

	if (res != cgltf_result_success)
		goto TERMINATE_FUNCTION;

	res = cgltf_load_buffers(&Options, pData, pFilename);
	LE_CHECK_CGLTF_RESULT(res);

	if (res != cgltf_result_success)
		goto TERMINATE_FUNCTION_AND_FREE;




TERMINATE_FUNCTION_AND_FREE:
	cgltf_free(pData);
TERMINATE_FUNCTION:
	return pScene;
}