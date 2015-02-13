import std.json;
import std.stdio;
import std.conv;
import std.file;

float[3] decodeVec3(JSONValue v)
{
	float[3] vec;

	for(int i=0;i<3;i++)
	{
		auto s = to!string(v.array[i]);
		vec[i] = to!float(s);
	}

	return vec;
}

float[16] decodeMat4(JSONValue v)
{
	float[16] mat;

	for(int i=0;i<16;i++)
	{
		auto s = to!string(v.array[i]);
		mat[i] = to!float(s);
	}

	return mat;
}

string toString(int n)(float[n] v)
{
	string cat = "";
	for(int i=0;i<n;i++)
		cat ~= to!string(v[i])~" ";
	return cat;
}

int main(string[] argv)
{
	auto content = to!string(read("hellknight_gangnam_style.json"));
	JSONValue json = parseJSON(content);

	auto vertices = json.object["model"].object["vertices"];
	auto indices_mesh0 = json.object["model"].object["meshes"].array[0].object["indices"].array;
	auto indices_mesh1 = json.object["model"].object["meshes"].array[1].object["indices"].array;

	auto positions = vertices.array[0].object["position"].object["data"].array;
	auto normals = vertices.array[0].object["normal"].object["data"].array;
	auto blend_indices = vertices.array[0].object["blendIndices"].object["data"].array;
	auto blend_weights = vertices.array[0].object["blendWeight"].object["data"].array;
	auto tex0 = vertices.array[0].object["texCoord0"].object["data"].array;
	auto tex1 = vertices.array[0].object["texCoord1"].object["data"].array;
	
	File file = File("hellknight.ply", "w");
	file.writeln("ply");
	file.writeln("format ascii 1.0");
	file.writeln("element vertex " ~ to!string(positions.length / 3));
	file.writeln("property float x");
	file.writeln("property float y");
	file.writeln("property float z");
	file.writeln("property float nx");
	file.writeln("property float ny");
	file.writeln("property float nz");
	file.writeln("property float s");
	file.writeln("property float t");
	file.writeln("property float u");
	file.writeln("property float v");
	file.writeln("property uint8 skinBlend0");
	file.writeln("property uint8 skinBlend1");
	file.writeln("property uint8 skinBlend2");
	file.writeln("property uint8 skinBlend3");
	file.writeln("property float skinWeight0");
	file.writeln("property float skinWeight1");
	file.writeln("property float skinWeight2");
	file.writeln("property float skinWeight3");
	file.writeln("element face " ~ to!string(indices_mesh0.length /3 ));
	file.writeln("property list uchar int vertex_indices");
	file.writeln("end_header");


	for(int i = 0; i < positions.length / 3; i++)
	{
		auto p = to!string(positions[i*3+0]) ~ " " ~ to!string(positions[i*3+1]) ~ " " ~ to!string(positions[i*3+2]);
		auto n = to!string(normals[i*3+0]) ~ " " ~ to!string(normals[i*3+1]) ~ " " ~ to!string(normals[i*3+2]);
		auto tc0 = to!string(tex0[i*2+0]) ~ " " ~ to!string(tex0[i*2+1]);
		auto tc1 = to!string(tex1[i*2+0]) ~ " " ~ to!string(tex1[i*2+1]);
		auto blendIdx = to!string(blend_indices[i*4+0]) ~ " " ~ to!string(blend_indices[i*4+1])~ " " ~ to!string(blend_indices[i*4+2])~ " " ~ to!string(blend_indices[i*4+3]);
		auto blendWeight = to!string(blend_weights[i*4+0]) ~ " " ~ to!string(blend_weights[i*4+1])~ " " ~ to!string(blend_weights[i*4+2])~ " " ~ to!string(blend_weights[i*4+3]);

		file.writeln(p ~ " " ~ n ~ " " ~ tc0 ~ " " ~ tc1 ~ " " ~ blendIdx ~ " " ~ blendWeight);
	}

	for(int i = 0; i < indices_mesh0.length / 3; i++)
	{
		file.writeln("3 " ~ to!string(indices_mesh0[i*3+0]) ~ " " ~to!string(indices_mesh0[i*3+1]) ~ " " ~to!string(indices_mesh0[i*3+2]) );
	}

	auto nodes = json.object["model"].object["nodes"].array;
	auto parents = json.object["model"].object["parents"];
	auto inverseBindMatrices = json.object["model"].object["skins"].array[0].object["inverseBindMatrices"].array;
	auto boneNames = json.object["model"].object["skins"].array[0].object["boneNames"].array;
	File skel = File("hellknight.skel", "w");
	
	for(int i = 0; i < boneNames.length; i++)
	{
		auto boneName = boneNames[i].str[10..$];
		auto invBindPose = decodeMat4(inverseBindMatrices[i]);

		skel.writeln(boneName ~ " " ~ invBindPose.toString);
	}

	/*for(int i = 0; i < nodes.length; i++)
	{
	auto nodename = nodes[i].object["name"].str;
	float[3] position = decodeVec3(nodes[i].object["position"]);
	float[3] rotation = decodeVec3(nodes[i].object["rotation"]);
	float[3] scale = decodeVec3(nodes[i].object["scale"]);

	//float[16] inverseBindMatrix = decodeMat4()

	writeln(nodename ~ " " ~ position.toString ~ " " ~ rotation.toString ~ " " ~ scale.toString);
	}*/

	content = to!string(read("hellknight_gangnam_style_animation.json"));
	json = parseJSON(content);

	File anim = File("hellknight_gangnamstyle.anim", "w");
	nodes = json.object["animation"].object["nodes"].array;
	auto duration = json.object["animation"].object["duration"];

	anim.writeln(to!string(duration));
	for(int i = 0; i < nodes.length; i++)
	{
		auto nodeName = nodes[i].object["name"].str[10..$];
		auto keys = nodes[i].object["keys"].array;
		anim.writeln(nodeName);

		for(int k=0;k<keys.length;k++)
		{
			// position of hips ignored
			auto key = keys[k];
			auto time = key.object["t"];
			//auto p =  decodeVec3(key.object["p"]);
			auto r =  decodeVec3(key.object["r"]);
			anim.writeln(to!string(time) ~ " " ~ r.toString);
		}
	}

	//writeln(json.object["key"].object["subkey2"].array[1].integer);
	//writeln(json.object["key"].object["subkey3"].type == JSON_TYPE.FLOAT);
	readln();
    return 0;
}
