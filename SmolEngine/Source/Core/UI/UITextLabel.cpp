#include "stdafx.h"
#include "UITextLabel.h"

#include "Core/Renderer/Text.h"
#include "Core/ECS/WorldAdmin.h"
#include "Core/SLog.h"

namespace SmolEngine
{
    UITextLabel::UITextLabel()
    {

    }

    void UITextLabel::Init(const std::string& fontFilePath, const std::string& fontName)
    {
        m_TextLabel = Text::Create(fontFilePath);

        m_FontFilePath = fontFilePath;
        m_FontName = fontName;
    }

    void UITextLabel::SetText(const std::string& text)
    {
        m_Text = text;

        if (m_TextLabel) 
        {
            m_TextLabel->SetText(text);
        }
    }

    void UITextLabel::SetFont(const std::string& fontFilePath)
    {
        if (m_TextLabel)
        {
            m_TextLabel->SetFont(fontFilePath);
        }
    }

    void UITextLabel::SetSize(const glm::vec2& size)
    {
        m_Size = size;
    }

    void UITextLabel::SetColor(const glm::vec4& color)
    {
        m_Color = color;
    }

    void UITextLabel::SetPosition(const glm::vec2& pos)
    {
        m_Position = pos;
    }

    void UITextLabel::Reload()
    {
        m_TextLabel = nullptr;

        auto& assetMap = WorldAdmin::GetSingleton()->GetActiveScene().GetSceneData().m_AssetMap;

        auto& result = assetMap.find(m_FontName);
        if (result != assetMap.end())
        {
            m_TextLabel = Text::Create(result->second);
            m_TextLabel->SetText(m_Text);
            return;
        }

        NATIVE_ERROR("TextLabel: font file not found, path: {}", m_FontFilePath);
    }

    void UITextLabel::Draw(const glm::vec3& cameraPos)
    {
        if (m_TextLabel)
        {
            m_TextLabel->Render(glm::vec3(cameraPos.x + m_Position.x, cameraPos.y + m_Position.y, 1.0f ), m_Size, m_Color, m_Padding);
        }
    }
}