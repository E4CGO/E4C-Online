#include "AssimpImporter.h"

#include <fstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "TAKOEngine/Rendering/Misc.h"

// コンストラクタ
AssimpImporter::AssimpImporter(const char* filename)
	: filepath(filename)
{
	// 拡張子取得
	std::string extension = filepath.extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);	// 小文字化

	// FBXファイルの場合は特殊なインポートオプション設定をする
	if (extension == ".fbx")
	{
		// $AssimpFBX$が付加された余計なノードを作成してしまうのを抑制する
		aImporter.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	}

	// インポート時のオプションフラグ
	uint32_t aFlags = 0
		//| aiProcess_Triangulate							// 多角形を三角形化する
		| aiProcess_JoinIdenticalVertices				// 重複頂点をマージする
		| aiProcess_PopulateArmatureData				// ボーンの参照データを取得できるようにする
		//| aiProcess_OptimizeMeshes
		//| aiProcess_OptimizeGraph
		//| aiProcess_FixInfacingNormals
		| aiProcess_CalcTangentSpace;					// 接線を計算する

	// ファイル読み込み
	aScene = aImporter.ReadFile(filename, aFlags);
	_ASSERT_EXPR_A(aScene, aImporter.GetErrorString());
}

// ノードデータを読み込み
void AssimpImporter::LoadNodes(NodeList& nodes)
{
	LoadNodes(nodes, aScene->mRootNode, -1);
}

// メッシュデータを読み込み
void AssimpImporter::LoadMeshes(MeshList& meshes, const NodeList& nodes)
{
	LoadMeshes(meshes, nodes, aScene->mRootNode);
}

// マテリアルデータを読み込み
void AssimpImporter::LoadMaterials(MaterialList& materials)
{
	// ディレクトリパス取得
	std::filesystem::path dirpath(filepath.parent_path());

	materials.resize(aScene->mNumMaterials);
	for (uint32_t aMaterialIndex = 0; aMaterialIndex < aScene->mNumMaterials; ++aMaterialIndex)
	{
		const aiMaterial* aMaterial = aScene->mMaterials[aMaterialIndex];
		ModelResource::Material& material = materials.at(aMaterialIndex);

		// マテリアル名
		aiString aMaterialName;
		aMaterial->Get(AI_MATKEY_NAME, aMaterialName);
		material.name = aMaterialName.C_Str();

		// ディフューズ色
		aiColor3D aDiffuseColor;
		if (AI_SUCCESS == aMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aDiffuseColor))
		{
			material.color = aiColor3DToXMFLOAT4(aDiffuseColor);
		}

		// テクスチャ読み込み関数
		auto loadTexture = [&](aiTextureType aTextureType, std::string& textureFilename)
			{
				// テクスチャファイルパス取得
				aiString aTextureFilePath;
				if (AI_SUCCESS == aMaterial->GetTexture(aTextureType, 0, &aTextureFilePath))
				{
					// 埋め込みテクスチャか確認
					const aiTexture* aTexture = aScene->GetEmbeddedTexture(aTextureFilePath.C_Str());
					if (aTexture != nullptr)
					{
						// テクスチャファイルパス作成
						std::filesystem::path textureFilePath(aTextureFilePath.C_Str());
						if (textureFilePath == "*0")
						{
							// テクスチャファイル名がなかった場合はマテリアル名とテクスチャタイプから作成
							textureFilePath = material.name + "_" + aiTextureTypeToString(aTextureType) + "." + aTexture->achFormatHint;
						}
						textureFilePath = "Textures" / textureFilePath.filename();

						// 埋め込みテクスチャを出力するディレクトリを確認
						std::filesystem::path outputDirPath(dirpath / textureFilePath.parent_path());
						if (!std::filesystem::exists(outputDirPath))
						{
							// なかったらディレクトリ作成
							std::filesystem::create_directories(outputDirPath);
						}
						// 出力ディレクトリに画像ファイルを保存
						std::filesystem::path outputFilePath(dirpath / textureFilePath);
						if (!std::filesystem::exists(outputFilePath))
						{
							// mHeightが0の場合は画像の生データなのでそのままバイナリ出力
							if (aTexture->mHeight == 0)
							{
								std::ofstream os(outputFilePath.string().c_str(), std::ios::binary);
								os.write(reinterpret_cast<char*>(aTexture->pcData), aTexture->mWidth);
							}
							else
							{
								// リニアな画像データは.pngで出力
								outputFilePath.replace_extension(".png");
								stbi_write_png(
									outputFilePath.string().c_str(),
									static_cast<int>(aTexture->mWidth),
									static_cast<int>(aTexture->mHeight),
									static_cast<int>(sizeof(uint32_t)),
									aTexture->pcData, 0);
							}
						}
						// テクスチャファイルパスを格納
						textureFilename = textureFilePath.string();
					}
					else
					{
						// テクスチャファイルパスをそのまま格納
						textureFilename = aTextureFilePath.C_Str();
					}
				}
			};

		// ディフューズマップ
		loadTexture(aiTextureType_DIFFUSE, material.diffuseTextureFileName);

		// ノーマルマップ
		loadTexture(aiTextureType_NORMALS, material.normalTextureFileName);
	}
}

// ノードデータを再帰読み込み
void AssimpImporter::LoadNodes(NodeList& nodes, const aiNode* aNode, int parentIndex)
{
	// aiNode*からModel::Nodeのインデックスを取得できるようにする
	std::map<const aiNode*, int>::iterator it = nodeIndexMap.find(aNode);
	if (it == nodeIndexMap.end())
	{
		nodeIndexMap[aNode] = static_cast<int>(nodes.size());
	}

	// トランスフォームデータ取り出し
	aiVector3D aScale, aPosition;
	aiQuaternion aRotation;
	aNode->mTransformation.Decompose(aScale, aRotation, aPosition);

	// ノードデータ格納
	ModelResource::Node& node = nodes.emplace_back();
	node.name = aNode->mName.C_Str();
	node.parentIndex = parentIndex;
	node.scale = aiVector3DToXMFLOAT3(aScale);
	node.rotation = aiQuaternionToXMFLOAT4(aRotation);
	node.position = aiVector3DToXMFLOAT3(aPosition);

	parentIndex = static_cast<int>(nodes.size() - 1);

	// 再帰的に子ノードを処理する
	for (uint32_t aNodeIndex = 0; aNodeIndex < aNode->mNumChildren; ++aNodeIndex)
	{
		LoadNodes(nodes, aNode->mChildren[aNodeIndex], parentIndex);
	}
}

// メッシュデータを読み込み
void AssimpImporter::LoadMeshes(MeshList& meshes, const NodeList& nodes, const aiNode* aNode)
{
	// メッシュデータ読み取り
	for (uint32_t aMeshIndex = 0; aMeshIndex < aNode->mNumMeshes; ++aMeshIndex)
	{
		const aiMesh* aMesh = aScene->mMeshes[aNode->mMeshes[aMeshIndex]];

		// メッシュデータ格納
		ModelResource::Mesh& mesh = meshes.emplace_back();
		mesh.vertices.resize(aMesh->mNumVertices);
		mesh.indices.resize(aMesh->mNumFaces * 3);
		mesh.materialIndex = static_cast<int>(aMesh->mMaterialIndex);
		mesh.nodeIndex = nodeIndexMap[aNode];

		// 頂点データ
		for (uint32_t aVertexIndex = 0; aVertexIndex < aMesh->mNumVertices; ++aVertexIndex)
		{
			ModelResource::Vertex& vertex = mesh.vertices.at(aVertexIndex);
			// 位置
			if (aMesh->HasPositions())
			{
				vertex.position = aiVector3DToXMFLOAT3(aMesh->mVertices[aVertexIndex]);
			}
			// テクスチャ座標
			if (aMesh->HasTextureCoords(0))
			{
				vertex.texcoord = aiVector3DToXMFLOAT2(aMesh->mTextureCoords[0][aVertexIndex]);
				vertex.texcoord.y = 1.0f - vertex.texcoord.y;
			}
			// 法線
			if (aMesh->HasNormals())
			{
				vertex.normal = aiVector3DToXMFLOAT3(aMesh->mNormals[aVertexIndex]);
			}
			// 接線
			if (aMesh->HasTangentsAndBitangents())
			{
				vertex.tangent = aiVector3DToXMFLOAT3(aMesh->mTangents[aVertexIndex]);
			}
		}

		// インデックスデータ
		for (uint32_t aFaceIndex = 0; aFaceIndex < aMesh->mNumFaces; ++aFaceIndex)
		{
			const aiFace& aFace = aMesh->mFaces[aFaceIndex];
			uint32_t index = aFaceIndex * 3;
			mesh.indices[index + 0] = aFace.mIndices[2];
			mesh.indices[index + 1] = aFace.mIndices[1];
			mesh.indices[index + 2] = aFace.mIndices[0];
		}

		// スキニングデータ
		if (aMesh->mNumBones > 0)
		{
			// ボーン影響力データ
			struct BoneInfluence
			{
				uint32_t	indices[4] = { 0, 0, 0, 0 };
				float		weights[4] = { 1, 0, 0, 0 };
				int			useCount = 0;

				void Add(uint32_t index, float weight)
				{
					if (useCount >= 4)
					{
						for (int i = 0; i < 4; ++i)
						{
							if (weights[i] == 0)
							{
								weights[i] = weight;
								indices[i] = index;
								break;
							}
						}
						return;
					}

					for (int i = 0; i < useCount; ++i)
					{
						if (indices[i] == index)
						{
							return;
						}
					}
					indices[useCount] = index;
					weights[useCount] = weight;
					useCount++;
				}

				void Normalize()
				{
					float total = 0;
					for (int i = 0; i < 4; ++i)
					{
						total += weights[i];
					}
					for (int i = 0; i < 4; ++i)
					{
						weights[i] /= total;
					}
				}
			};
			std::vector<BoneInfluence> boneInfluences;
			boneInfluences.resize(aMesh->mNumVertices);

			// メッシュに影響するボーンデータを収集する
			for (uint32_t aBoneIndex = 0; aBoneIndex < aMesh->mNumBones; ++aBoneIndex)
			{
				const aiBone* aBone = aMesh->mBones[aBoneIndex];

				// 頂点影響力データを抽出
				for (uint32_t aWightIndex = 0; aWightIndex < aBone->mNumWeights; ++aWightIndex)
				{
					const aiVertexWeight& aWeight = aBone->mWeights[aWightIndex];
					BoneInfluence& boneInfluence = boneInfluences.at(aWeight.mVertexId);
					boneInfluence.Add(aBoneIndex, aWeight.mWeight);
				}

				// ボーンデータ取得
				ModelResource::Bone& bone = mesh.bones.emplace_back();
				bone.nodeIndex = nodeIndexMap[aBone->mNode];
				bone.offsetTransform = aiMatrix4x4ToXMFLOAT4X4(aBone->mOffsetMatrix);
			}
			for (BoneInfluence& boneInfluence : boneInfluences)
			{
				boneInfluence.Normalize();
			}

			// 頂点影響力データを格納
			for (size_t vertexIndex = 0; vertexIndex < mesh.vertices.size(); ++vertexIndex)
			{
				ModelResource::Vertex& vertex = mesh.vertices.at(vertexIndex);
				BoneInfluence& boneInfluence = boneInfluences.at(vertexIndex);
				vertex.boneWeight.x = boneInfluence.weights[0];
				vertex.boneWeight.y = boneInfluence.weights[1];
				vertex.boneWeight.z = boneInfluence.weights[2];
				vertex.boneWeight.w = boneInfluence.weights[3];
				vertex.boneIndex.x = boneInfluence.indices[0];
				vertex.boneIndex.y = boneInfluence.indices[1];
				vertex.boneIndex.z = boneInfluence.indices[2];
				vertex.boneIndex.w = boneInfluence.indices[3];
			}
		}
	}

	// 再帰的に子ノードを処理する
	for (uint32_t aNodeIndex = 0; aNodeIndex < aNode->mNumChildren; ++aNodeIndex)
	{
		LoadMeshes(meshes, nodes, aNode->mChildren[aNodeIndex]);
	}
}

// ノードインデックス取得
int AssimpImporter::GetNodeIndex(const NodeList& nodes, const char* name)
{
	int index = 0;
	for (const ModelResource::Node& node : nodes)
	{
		if (node.name == name)
		{
			return index;
		}
		index++;
	}
	return -1;
}

// アニメーションデータを読み込み
void AssimpImporter::LoadAnimations(AnimationList& animations, const NodeList& nodes)
{
	for (uint32_t aAnimationIndex = 0; aAnimationIndex < aScene->mNumAnimations; ++aAnimationIndex)
	{
		const aiAnimation* aAnimation = aScene->mAnimations[aAnimationIndex];
		ModelResource::Animation& animation = animations.emplace_back();

		// アニメーション情報
		animation.name = aAnimation->mName.C_Str();
		animation.secondsLength = static_cast<float>(aAnimation->mDuration / aAnimation->mTicksPerSecond);

		// ノード毎のアニメーション
		animation.nodeAnims.resize(nodes.size());
		for (uint32_t aChannelIndex = 0; aChannelIndex < aAnimation->mNumChannels; ++aChannelIndex)
		{
			const aiNodeAnim* aNodeAnim = aAnimation->mChannels[aChannelIndex];
			int nodeIndex = GetNodeIndex(nodes, aNodeAnim->mNodeName.C_Str());
			if (nodeIndex < 0) continue;

			const ModelResource::Node& node = nodes.at(nodeIndex);
			ModelResource::NodeAnim& nodeAnim = animation.nodeAnims.at(nodeIndex);

			// 位置
			for (uint32_t aPositionIndex = 0; aPositionIndex < aNodeAnim->mNumPositionKeys; ++aPositionIndex)
			{
				const aiVectorKey& aKey = aNodeAnim->mPositionKeys[aPositionIndex];
				// なぜかUnityで出力したアニメーションデータにはゴミと思われるキーフレームが存在している場合がある。
				// 小数点が存在するフレーム（時間）がゴミデータっぽいので除外する。
				if (fabs(std::round(aKey.mTime) - aKey.mTime) > 0.001) continue;
				ModelResource::VectorKeyframe& keyframe = nodeAnim.positionKeyframes.emplace_back();
				keyframe.seconds = static_cast<float>(aKey.mTime / aAnimation->mTicksPerSecond);
				keyframe.value = aiVector3DToXMFLOAT3(aKey.mValue);
			}
			// 回転
			for (uint32_t aRotationIndex = 0; aRotationIndex < aNodeAnim->mNumRotationKeys; ++aRotationIndex)
			{
				const aiQuatKey& aKey = aNodeAnim->mRotationKeys[aRotationIndex];
				// なぜかUnityで出力したアニメーションデータにはゴミと思われるキーフレームが存在している場合がある。
				// 小数点が存在するフレーム（時間）がゴミデータっぽいので除外する。
				if (fabs(std::round(aKey.mTime) - aKey.mTime) > 0.001) continue;
				ModelResource::QuaternionKeyframe& keyframe = nodeAnim.rotationKeyframes.emplace_back();
				keyframe.seconds = static_cast<float>(aKey.mTime / aAnimation->mTicksPerSecond);
				keyframe.value = aiQuaternionToXMFLOAT4(aKey.mValue);
			}
			// スケール
			for (uint32_t aScalingIndex = 0; aScalingIndex < aNodeAnim->mNumScalingKeys; ++aScalingIndex)
			{
				const aiVectorKey& aKey = aNodeAnim->mScalingKeys[aScalingIndex];
				// なぜかUnityで出力したアニメーションデータにはゴミと思われるキーフレームが存在している場合がある。
				// 小数点が存在するフレーム（時間）がゴミデータっぽいので除外する。
				if (fabs(std::round(aKey.mTime) - aKey.mTime) > 0.001) continue;
				ModelResource::VectorKeyframe& keyframe = nodeAnim.scaleKeyframes.emplace_back();
				keyframe.seconds = static_cast<float>(aKey.mTime / aAnimation->mTicksPerSecond);
				keyframe.value = aiVector3DToXMFLOAT3(aKey.mValue);
			}

			// 全てのキーの値がほぼ同じ内容だった場合は削る
			DirectX::XMVECTOR Epsilon = DirectX::XMVectorReplicate(0.00001f);
			// 位置
			{
				bool result = true;
				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&nodeAnim.positionKeyframes.at(0).value);
				for (size_t i = 1; i < nodeAnim.positionKeyframes.size(); ++i)
				{
					DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&nodeAnim.positionKeyframes.at(i).value);
					if (!DirectX::XMVector3NearEqual(A, B, Epsilon))
					{
						result = false;
						break;
					}
				}
				if (result)
				{
					nodeAnim.positionKeyframes.resize(1);
				}
			}
			// 回転
			{
				bool result = true;
				DirectX::XMVECTOR A = DirectX::XMLoadFloat4(&nodeAnim.rotationKeyframes.at(0).value);
				for (size_t i = 1; i < nodeAnim.rotationKeyframes.size(); ++i)
				{
					DirectX::XMVECTOR B = DirectX::XMLoadFloat4(&nodeAnim.rotationKeyframes.at(i).value);
					if (!DirectX::XMVector4NearEqual(A, B, Epsilon))
					{
						result = false;
						break;
					}
				}
				if (result)
				{
					nodeAnim.rotationKeyframes.resize(1);
				}
			}
			// スケール
			{
				bool result = true;
				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&nodeAnim.scaleKeyframes.at(0).value);
				for (size_t i = 1; i < nodeAnim.scaleKeyframes.size(); ++i)
				{
					DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&nodeAnim.scaleKeyframes.at(i).value);
					if (!DirectX::XMVector3NearEqual(A, B, Epsilon))
					{
						result = false;
						break;
					}
				}
				if (result)
				{
					nodeAnim.scaleKeyframes.resize(1);
				}
			}
		}
		// アニメーションがなかったノードに対して初期姿勢のキーフレームを追加する
		for (size_t nodeIndex = 0; nodeIndex < animation.nodeAnims.size(); ++nodeIndex)
		{
			const ModelResource::Node& node = nodes.at(nodeIndex);
			ModelResource::NodeAnim& nodeAnim = animation.nodeAnims.at(nodeIndex);
			// 移動
			if (nodeAnim.positionKeyframes.size() == 0)
			{
				ModelResource::VectorKeyframe& keyframe = nodeAnim.positionKeyframes.emplace_back();
				keyframe.seconds = 0.0f;
				keyframe.value = node.position;
			}
			if (nodeAnim.positionKeyframes.size() == 1)
			{
				ModelResource::VectorKeyframe& keyframe = nodeAnim.positionKeyframes.emplace_back();
				keyframe.seconds = animation.secondsLength;
				keyframe.value = nodeAnim.positionKeyframes.at(0).value;
			}
			// 回転
			if (nodeAnim.rotationKeyframes.size() == 0)
			{
				ModelResource::QuaternionKeyframe& keyframe = nodeAnim.rotationKeyframes.emplace_back();
				keyframe.seconds = 0.0f;
				keyframe.value = node.rotation;
			}
			if (nodeAnim.rotationKeyframes.size() == 1)
			{
				ModelResource::QuaternionKeyframe& keyframe = nodeAnim.rotationKeyframes.emplace_back();
				keyframe.seconds = animation.secondsLength;
				keyframe.value = nodeAnim.rotationKeyframes.at(0).value;
			}
			// スケール
			if (nodeAnim.scaleKeyframes.size() == 0)
			{
				ModelResource::VectorKeyframe& keyframe = nodeAnim.scaleKeyframes.emplace_back();
				keyframe.seconds = 0.0f;
				keyframe.value = node.scale;
			}
			if (nodeAnim.scaleKeyframes.size() == 1)
			{
				ModelResource::VectorKeyframe& keyframe = nodeAnim.scaleKeyframes.emplace_back();
				keyframe.seconds = animation.secondsLength;
				keyframe.value = nodeAnim.scaleKeyframes.at(0).value;
			}
		}
	}
}

// aiVector3D → XMFLOAT2
DirectX::XMFLOAT2 AssimpImporter::aiVector3DToXMFLOAT2(const aiVector3D& aValue)
{
	return DirectX::XMFLOAT2(
		static_cast<float>(aValue.x),
		static_cast<float>(aValue.y)
	);
}

// aiVector3D → XMFLOAT3
DirectX::XMFLOAT3 AssimpImporter::aiVector3DToXMFLOAT3(const aiVector3D& aValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(aValue.x),
		static_cast<float>(aValue.y),
		static_cast<float>(aValue.z)
	);
}

// aiColor3D → XMFLOAT4
DirectX::XMFLOAT4 AssimpImporter::aiColor3DToXMFLOAT4(const aiColor3D& aValue)
{
	return DirectX::XMFLOAT4(
		static_cast<float>(aValue.r),
		static_cast<float>(aValue.g),
		static_cast<float>(aValue.b),
		1.0f
	);
}

// aiQuaternion → XMFLOAT4
DirectX::XMFLOAT4 AssimpImporter::aiQuaternionToXMFLOAT4(const aiQuaternion& aValue)
{
	return DirectX::XMFLOAT4(
		static_cast<float>(aValue.x),
		static_cast<float>(aValue.y),
		static_cast<float>(aValue.z),
		static_cast<float>(aValue.w)
	);
}

// aiMatrix4x4 → XMFLOAT4X4
DirectX::XMFLOAT4X4 AssimpImporter::aiMatrix4x4ToXMFLOAT4X4(const aiMatrix4x4& aValue)
{
	return DirectX::XMFLOAT4X4(
		static_cast<float>(aValue.a1),
		static_cast<float>(aValue.b1),
		static_cast<float>(aValue.c1),
		static_cast<float>(aValue.d1),
		static_cast<float>(aValue.a2),
		static_cast<float>(aValue.b2),
		static_cast<float>(aValue.c2),
		static_cast<float>(aValue.d2),
		static_cast<float>(aValue.a3),
		static_cast<float>(aValue.b3),
		static_cast<float>(aValue.c3),
		static_cast<float>(aValue.d3),
		static_cast<float>(aValue.a4),
		static_cast<float>(aValue.b4),
		static_cast<float>(aValue.c4),
		static_cast<float>(aValue.d4)
	);
}