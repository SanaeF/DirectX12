#pragma once
#include <DirectXMath.h>
#include <d3dx12.h>
#include <memory>
#include <vector>
namespace lib {
	struct ModelParam {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 normal;
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT2 tex;

		// 追加UV座標
		std::vector<DirectX::XMFLOAT4> addUV;

		// ボーンウェイト
		struct Weight
		{
			enum Type
			{
				BDEF1,
				BDEF2,
				BDEF4,
				SDEF,
			};

			Type type;
			int born1;
			int	born2;
			int	born3;
			int	born4;
			float weight1;
			float weight2;
			float weight3;
			float weight4;
			DirectX::XMFLOAT3 c;
			DirectX::XMFLOAT3 r0;
			DirectX::XMFLOAT3 r1;
		} weight;

		// エッジ倍率
		float edgeMagnif;
	};
	struct Material
	{
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT3 specular;
		float specularity;
		DirectX::XMFLOAT3 ambient;

		int colorMapTextureIndex;
		int toonTextureIndex;
		// (スフィアテクスチャは非対応)

		// 材質ごとの頂点数
		int vertexNum;
	};
	struct Bone
	{
		// ボーン名
		std::wstring name;
		// English version
		std::string nameEnglish;
		DirectX::XMFLOAT3 position;
		int parentIndex;
		int transformationLevel;
		unsigned short flag;
		DirectX::XMFLOAT3 coordOffset;
		int childrenIndex;
		int impartParentIndex;
		float impartRate;
		// 固定軸方向ベクトル
		DirectX::XMFLOAT3 fixedAxis;
		// ローカルのX軸方向ベクトル
		DirectX::XMFLOAT3 localAxisX;
		// ローカルのZ軸方向ベクトル
		DirectX::XMFLOAT3 localAxisZ;
		int externalParentKey;
		int ikTargetIndex;
		int ikLoopCount;
		float ikUnitAngle;
		struct IKLink
		{
			int index;
			bool existAngleLimited;
			DirectX::XMFLOAT3 limitAngleMin;
			DirectX::XMFLOAT3 limitAngleMax;
		};
		std::vector<IKLink> ikLinks;
	};
	class ModelData {
	public:
		static ModelData Object;
		std::vector<lib::ModelParam>vertex;
		std::vector<UINT>index;
		std::vector<std::vector<int>>index_group;
		std::vector<std::wstring> texturePaths;
		std::vector<Material> materials;
		std::vector<Bone> bones;
		ID3D12Resource* vertex_buffer;
		ID3D12Resource* index_buffer;
		D3D12_VERTEX_BUFFER_VIEW vb_view;
		D3D12_INDEX_BUFFER_VIEW ib_view;
	private:
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		CD3DX12_RANGE m_Read_range;
	public:
		ModelData();
		void createViewBuffer(ComPtr<ID3D12Device> device);
	private:
		bool createVertexBuffer(ComPtr<ID3D12Device> device);
		bool createIndexBuffer(ComPtr<ID3D12Device> device);
	};
}
