#pragma once
#include "Scene.h"
#include <memory>

class CoreGraphicsManager; // Forward Declaration

class SceneManager
{
public:
    static SceneManager* GetInstance();

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    void Initialize(CoreGraphicsManager* graphics, int windowWidth, int windowHeight);
    void LoadScene(const std::string& name);
    void Update();
	void PrevRender( );
    void Render();
	void PostRender( );
	void OnGUIRender( );

    void OnDestroy();
    void ProcessPendingChanges();

    Scene* GetActiveScene() const { return m_activeScene.get(); }


	void ResetAllComponentSortState( )
	{
		m_ISASESorting = false;
		m_ISDESCSorting = false;
	}
protected:
	void UpdateAllComponent_ASESort( );
	void UpdateAllComponent_DESCSort( );

protected:
	bool m_ISASESorting = false;
	bool m_ISDESCSorting = false;
	std::vector<class Component* > m_ASESortedComponentList;
	std::vector<class Component* > m_DESCSortedComponentList;

private:
    SceneManager();
    ~SceneManager();

    static SceneManager* s_instance;
    std::unique_ptr<Scene> m_activeScene;
    CoreGraphicsManager* m_graphics;
    int m_windowWidth;
    int m_windowHeight;
};
