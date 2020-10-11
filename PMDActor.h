#pragma once

#include<d3d12.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<unordered_map>
#include<wrl.h>
#include<memory>

//class DXPMDBone;
class DXVMDMotion;

class DXPMDModel;
class Dx12Wrapper;
class PMDRenderer;

class PMDActor {
	friend PMDRenderer;
private:

	PMDRenderer& _renderer;
	Dx12Wrapper& _dx12;

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	//std::shared_ptr<DXPMDModel> VMD_model;
	//std::shared_ptr<DXPMDBone>Bone;

	std::shared_ptr<DXVMDMotion>Motion;

	//頂点関連
	ComPtr<ID3D12Resource> _vb = nullptr;
	ComPtr<ID3D12Resource> _ib = nullptr;
	D3D12_VERTEX_BUFFER_VIEW _vbView = {};
	D3D12_INDEX_BUFFER_VIEW _ibView = {};

	ComPtr<ID3D12Resource> _transformMat = nullptr;//座標変換行列(今はワールドのみ)

	//シェーダ側に投げられるマテリアルデータ
	struct MaterialForHlsl {
		DirectX::XMFLOAT3 diffuse; //ディフューズ色
		float alpha; // ディフューズα
		DirectX::XMFLOAT3 specular; //スペキュラ色
		float specularity;//スペキュラの強さ(乗算値)
		DirectX::XMFLOAT3 ambient; //アンビエント色
	};
	//それ以外のマテリアルデータ
	struct AdditionalMaterial {
		std::string texPath;//テクスチャファイルパス
		int toonIdx; //トゥーン番号
		bool edgeFlg;//マテリアル毎の輪郭線フラグ
	};

	//まとめたもの
	struct Material {
		unsigned int indicesNum;//インデックス数
		MaterialForHlsl material;
		AdditionalMaterial additional;
	};

	//マテリアル関連
	std::vector<Material> _materials;
	ComPtr<ID3D12Resource> _materialBuff = nullptr;
	std::vector<ComPtr<ID3D12Resource>> _textureResources;
	std::vector<ComPtr<ID3D12Resource>> _sphResources;
	std::vector<ComPtr<ID3D12Resource>> _spaResources;
	std::vector<ComPtr<ID3D12Resource>> _toonResources;


	//読み込んだマテリアルをもとにマテリアルバッファを作成
	HRESULT CreateMaterialData();

	ComPtr< ID3D12DescriptorHeap> _materialHeap = nullptr;//マテリアルヒープ(5個ぶん)
	//マテリアル＆テクスチャのビューを作成
	HRESULT CreateMaterialAndTextureView();


	//PMDファイルのロード
	HRESULT LoadPMDFile(const char* path);

	DWORD _startTime;

	void MotionUpdate();


	float _angle;//テスト用Y軸回転

public:
	PMDActor(const char* filepath, PMDRenderer& renderer);
	~PMDActor();

	///クローンは頂点およびマテリアルは共通のバッファを見るようにする

	PMDActor* Clone();

	void LoadVMDFile(const char* filepath, const char* name);
	void PlayAnimation();
	void Update();
	void Draw();

};