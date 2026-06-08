#pragma once

#include <cstdint>
#include <memory>
#include <render/renderEnum/TextureFormat.h>
#include <render/renderEnum/WrapMode.h>
#include <utils/memory/RefCounted.h>
#include <utils/Config.h>
#include <jsvm/JSVM_Types.h>

namespace laya
{
    class CONCH_EXPORT Texture2D : public RefCounted<Texture2D>
    {
    private:
        friend class RefCounted<Texture2D>;
        ~Texture2D();

        struct Impl;
        std::unique_ptr<Impl> m_impl;
        Texture2D();
        Texture2D(int width, int height, TextureFormat format, int mipmapLevel);
    public:
        /**
         * 重置纹理参数
         * @param width 宽度
         * @param height 高度
         * @param format 格式
         * @param mipmapLevel mipmap 级别
         */
         //必需引擎初始化之后才能调用，否则返回null
        static Texture2D* create(int width, int height, TextureFormat format, int mipmapLevel);

        /**
         * 上传纹理数据
         * @param data 数据指针
         * @param level mipmap 级别
         * @param arrayIndex 数组索引
         */
        void uploadData(const uint8_t* data, uint32_t level, uint32_t arrayIndex);

        /**
         * 获取纹理宽度
         */
        int getWidth() const;

        /**
         * 获取纹理高度
         */
        int getHeight() const;

        /**
         * 增加引用计数
         */
        void addRef();

        /**
         * 释放引用
         */
        void release();

        /**
         * 设置纹理包装模式
         * @param wrapMode 包装模式
         */
        void setWrapMode(WarpMode wrapModeU, WarpMode wrapModeV, WarpMode wrapModeW);

        jsvm_value getInternalTextureJS();
    };
} // namespace laya
