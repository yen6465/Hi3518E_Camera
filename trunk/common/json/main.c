#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "json.h"


void json_file_write(char* filename, const char* data, int len)
{
	FILE* fd = NULL;

	fd = fopen(filename, "wb");

	fwrite(data, 1, len, fd);

	fclose(fd);
}

struct json_object* json_node_get(json_object* obj, const char* key, void** val)
{
	if(val == NULL || obj == NULL || key == NULL)
		return NULL;

	json_object *pval = NULL;
	enum json_type type;
  
	json_bool ret = json_object_object_get_ex(obj, key, &pval);
	if(ret)
	{
		type = json_object_get_type(pval);
		switch(type)
		{
		case json_type_array:
			*val = pval;
			break;
		case json_type_object:
			*val = pval;
			break;
		default:
			break;
		}
		
		return pval;
	}
	
	return NULL;

}

struct json_object* json_value_set(json_object* obj, const char* key, void* val)
{
	if(val == NULL || obj == NULL || key == NULL)
		return NULL;

	json_object *pval = NULL;
	enum json_type type;
  
	json_bool ret = json_object_object_get_ex(obj, key, &pval);
	if(ret)
	{
		type = json_object_get_type(pval);
		switch(type)
		{
		case json_type_string:
			json_object_set_string(pval, (char*)val);
			break;
		case json_type_int:
			json_object_set_int(pval, *((int*)val));
			break;
		case json_type_boolean:
			json_object_set_boolean(pval, *((json_bool*)val));
			break;
		case json_type_double:
			json_object_set_double(pval, *((double*)val));
			break;
		default:
			break;
		}
		
		return pval;
	}
	
	return NULL;

}

struct json_object* json_value_get(json_object* obj, const char* key, void* val)
{
	if(val == NULL || obj == NULL || key == NULL)
		return NULL;

	json_object *pval = NULL;
	enum json_type type;
  
	json_bool ret = json_object_object_get_ex(obj, key, &pval);
	if(ret)
	{
		type = json_object_get_type(pval);
		switch(type)
		{
		case json_type_string:
			strcpy(val, json_object_get_string(pval));
			break;
		case json_type_int:
			*((int*)val) = json_object_get_int(pval);
			break;
		case json_type_boolean:
			*((json_bool*)val) = json_object_get_boolean(pval);
			break;
		case json_type_double:
			*((double*)val) = json_object_get_double(pval);
			break;
		default:
			break;
		}
		
		return pval;
	}
	
	return NULL;
}


int main()
{
	struct json_object *j_req_info = NULL;
	struct json_object *j_body = NULL;
	struct json_object *j_content = NULL;
	j_req_info = json_object_new_object();
	if(j_req_info == NULL) 
	{
		printf("json_object_new_object j_req_info = NULL\n");
		return -1;
	}
	
	j_body = json_object_new_object();
	if(j_body == NULL) {
		json_object_put(j_req_info);
		printf("json_object_new_object j_body = NULL\n");
		return -1;
	}
	json_object_object_add(j_req_info, "Body", j_body);
	
	j_content = json_object_new_object();
	if(j_content == NULL) {
		json_object_put(j_req_info);
		printf("json_object_new_object j_content = NULL\n");
		return -1;
	}
	json_object_object_add(j_body, "Content", j_content);
	
	//content 内容
	json_object_object_add(j_content, "DeviceId", json_object_new_string("id_string"));
	//body 内容
	json_object_object_add(j_body, "From", json_object_new_string("from_string"));
    json_object_object_add(j_body, "To", json_object_new_string("to_string"));
	json_object_object_add(j_body, "Type", json_object_new_string("set"));
	json_object_object_add(j_body, "NameSpace", json_object_new_string("osstoken:write"));
	json_object_object_add(j_body, "int", json_object_new_int(1));
	json_object_object_add(j_body, "string", json_object_new_string("string1"));
	json_object_object_add(j_body, "boolean", json_object_new_boolean(0));
	json_object_object_add(j_body, "double", json_object_new_double(3.111222));
	//json
	json_object_object_add(j_req_info, "MessageType", json_object_new_string("CloudStore"));
	json_object_object_add(j_req_info, "int", json_object_new_int(0));
	json_object_object_add(j_req_info, "string", json_object_new_string("string"));
	json_object_object_add(j_req_info, "boolean", json_object_new_boolean(1));
	json_object_object_add(j_req_info, "double", json_object_new_double(3.111));
	
	const char *data_info =json_object_to_json_string_ext(j_req_info, JSON_C_TO_STRING_PRETTY);
	printf("get_oss_params data_info:\n%s\n",data_info);

	json_file_write("./json_file.txt", data_info, strlen(data_info));
//	json_object_to_file("./json_file2.txt", j_req_info);
	char str[1024] = {0};
	strcpy(str, data_info);

	while(1)
	{
		json_object *jso = json_tokener_parse(str);	
		
		int i_val = -1;
		json_value_get(jso, "int", &i_val);
		printf("j_req_info int %d\n", i_val);

		char string[1024] = {0};
		json_value_get(jso, "string", &string);
		printf("j_req_info string %s\n", string);

		double d_val = -1;
		json_value_get(jso, "double", &d_val);
		printf("j_req_info double %f\n", d_val);

		json_bool b_val = 0;
		json_value_get(jso, "boolean", &b_val);
		printf("j_req_info boolean %d\n", b_val);

		json_object* ojb_val = NULL;
		json_node_get(jso, "Body", (void*)(&ojb_val));

		i_val = 100;
		json_value_set(ojb_val, "int", &i_val);
		json_value_get(ojb_val, "int", &i_val);
		printf("ojb_val int %d\n", i_val);

		strcpy(string, "fuck");
		json_value_set(ojb_val, "string", string);
		json_value_get(ojb_val, "string", string);
		printf("ojb_val string %s\n", string);

		d_val = 1.111111;
		json_value_set(ojb_val, "double", &d_val);
		json_value_get(ojb_val, "double", &d_val);
		printf("ojb_val double %f\n", d_val);

		b_val = 1;
		json_value_set(ojb_val, "boolean", &b_val);
		json_value_get(ojb_val, "boolean", &b_val);
		printf("ojb_val boolean %d\n", b_val);

		usleep(20*1000);
		json_object_put(jso);
	}

	
	json_object_put(j_req_info);

//	json_file_write("./json_file1.txt", data_info, strlen(data_info)); //无数据 json_object_put释放了



//	while(1)
//	{
//		json_object *jso = json_tokener_parse(str);	
//		json_object_get_string(jso);
//		json_object_to_file("./json_file3.txt", jso);
//		json_object_put(jso);
//		continue;
//	}
	
	return 0;
}

