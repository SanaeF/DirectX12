#include "DxUploadTex2D.h"
#include "DxHelper.h"

#include"Dx12Wrapper.h"

void DxUploadTex2D::mUploadHeapProp() {
	mUploadHeap_Prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	mUploadHeap_Prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	mUploadHeap_Prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	mUploadHeap_Prop.CreationNodeMask = 0;
	mUploadHeap_Prop.VisibleNodeMask = 0;
}

void DxUploadTex2D::mUploadResourceDesc() {
	mResource_Desc.Format = DXGI_FORMAT_UNKNOWN;
	mResource_Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	mResource_Desc.Width = getImage().slicePitch;
	mResource_Desc.Height = 1;
	mResource_Desc.DepthOrArraySize = 1;
	mResource_Desc.MipLevels = 1;
	mResource_Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	mResource_Desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	mResource_Desc.SampleDesc.Count = 1;
	mResource_Desc.SampleDesc.Quality = 0;
}

void DxUploadTex2D::mCreateUploadResource(std::shared_ptr<Dx12Wrapper> DxWrap) {
	this->mUploadHeapProp();
	this->mUploadResourceDesc();

	auto result = DxWrap->Device()->CreateCommittedResource(
		&mUploadHeap_Prop,
		D3D12_HEAP_FLAG_NONE,
		&mResource_Desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mUploadBuff)
	);
	if (!SUCCEEDED(result)) {
		assert(0 && "UploadBuff Error!");
	}
}

void DxUploadTex2D::mTexHeapProp() {
	mTexHeap_Prop.Type = D3D12_HEAP_TYPE_DEFAULT;
	mTexHeap_Prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	mTexHeap_Prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	mTexHeap_Prop.CreationNodeMask = 0;
	mTexHeap_Prop.VisibleNodeMask = 0;
}

void DxUploadTex2D::mResourceProp() {
	mResource_Desc.Format = getMetaData().format;
	mResource_Desc.Width = getMetaData().width;
	mResource_Desc.Height = getMetaData().height;
	mResource_Desc.DepthOrArraySize = getMetaData().arraySize;
	mResource_Desc.MipLevels = getMetaData().mipLevels;
	mResource_Desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(getMetaData().dimension);
	mResource_Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
}


void DxUploadTex2D::CreateTexBuffer(std::shared_ptr<Dx12Wrapper> DxWrap) {
	mCreateUploadResource(DxWrap);
	mTexHeapProp();
	mResourceProp();

	auto result = DxWrap->Device()->CreateCommittedResource(
		&mTexHeap_Prop,
		D3D12_HEAP_FLAG_NONE,
		&mResource_Desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&mTextureBuff)
	);
	if (!SUCCEEDED(result)) {
		assert(0 && "TextureBuff Error!");
	}
}

void DxUploadTex2D::ResourceUpload() {
	DxHelper Helper;
	uint8_t* mMap_for_Image = nullptr;
	//auto srcAddress = getImage().pixels;
	//auto rowPitch = 
	//	Helper.AlignmentedSize(
	//	getImage().rowPitch,
	//	D3D12_TEXTURE_DATA_PITCH_ALIGNMENT
	//);
	//for (int y = 0; y < getImage().height; y++) {
	//	std::copy_n(//
	//		srcAddress,
	//		rowPitch,
	//		mMap_for_Image
	//	);
	//	srcAddress += getImage().rowPitch;
	//	mMap_for_Image += rowPitch;
	//}
	auto result = mUploadBuff->Map(0, nullptr, (void**)&mMap_for_Image);
	std::copy_n(getImage().pixels, getImage().slicePitch, mMap_for_Image);
	mUploadBuff->Unmap(0, nullptr);
}

void DxUploadTex2D::TextureCopy(std::shared_ptr<Dx12Wrapper> DxWrap) {
	DxHelper Helper;
	mSRC.pResource = mUploadBuff;
	mSRC.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
	UINT nrow;
	UINT64 rowsize, size;
	auto desc = mTextureBuff->GetDesc();
	DxWrap->Device()->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, &nrow, &rowsize, &size);
	mSRC.PlacedFootprint = footprint;
	mSRC.PlacedFootprint.Offset = 0;
	//mSRC.PlacedFootprint.Footprint.Width = getMetaData().width;

	mSRC.PlacedFootprint.Footprint.Width = 
		Helper.AlignmentedSize(
		getImage().rowPitch,
		D3D12_TEXTURE_DATA_PITCH_ALIGNMENT
		) * getImage().height;

	mSRC.PlacedFootprint.Footprint.Height = getMetaData().height;
	mSRC.PlacedFootprint.Footprint.Depth = getMetaData().depth;
	//mSRC.PlacedFootprint.Footprint.RowPitch = getImage().rowPitch;

	mSRC.PlacedFootprint.Footprint.RowPitch =
		Helper.AlignmentedSize(
			getImage().rowPitch,
			D3D12_TEXTURE_DATA_PITCH_ALIGNMENT
		);

	mSRC.PlacedFootprint.Footprint.Format = getImage().format;
	mDST.pResource = mTextureBuff;
	mDST.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	mDST.SubresourceIndex = 0;
}

void DxUploadTex2D::BarrierDescProp(std::shared_ptr<Dx12Wrapper> DxWrap) {
	DxWrap->CommandList()->CopyTextureRegion(
		&mDST, 0, 0, 0, &mSRC, nullptr
	);

	mBarrier_Desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mBarrier_Desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	mBarrier_Desc.Transition.pResource = mTextureBuff;
	mBarrier_Desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	mBarrier_Desc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	mBarrier_Desc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	DxWrap->CommandList()->ResourceBarrier(1, &mBarrier_Desc);
	DxWrap->CommandClear();

}

void DxUploadTex2D::ShaderResourceViewforUpload(std::shared_ptr<Dx12Wrapper> DxWrap) {
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	descHeapDesc.NodeMask = 0;//�}�X�N��0
	descHeapDesc.NumDescriptors = 1;//�r���[�͍��̂Ƃ���P����
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�V�F�[�_���\�[�X�r���[(����ђ萔�AUAV��)
	auto result = DxWrap->Device()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&mTexture_DescHeap));//����

	//�ʏ�e�N�X�`���r���[�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = getMetaData().format;//DXGI_FORMAT_R8G8B8A8_UNORM;//RGBA(0.0f�`1.0f�ɐ��K��)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;//��q
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;//�~�b�v�}�b�v�͎g�p���Ȃ��̂�1

	DxWrap->Device()->CreateShaderResourceView(
		mTextureBuff, //�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc, //��قǐݒ肵���e�N�X�`���ݒ���
		mTexture_DescHeap->GetCPUDescriptorHandleForHeapStart()//�q�[�v�̂ǂ��Ɋ��蓖�Ă邩
	);
}

ID3D12DescriptorHeap* 
DxUploadTex2D::getTexDescHeap() {
	return mTexture_DescHeap;
}
D3D12_RESOURCE_BARRIER 
DxUploadTex2D::getBarrier() {
	return mBarrier_Desc;
}
D3D12_TEXTURE_COPY_LOCATION 
DxUploadTex2D::getSRC() {
	return mSRC;
}
D3D12_TEXTURE_COPY_LOCATION 
DxUploadTex2D::getDST() {
	return mDST;
}