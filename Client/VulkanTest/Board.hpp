#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "lve_ecs.hpp"
#include "lve_components.hpp"
#include "lve_utils.hpp"
#include "lve_model.hpp"

class Board {
    static constexpr float BOARD_SIZE = 8.f;
public:
    Board(lve::Coordinator& coordinator, std::shared_ptr<lve::LveModel> model, int width, int height)
        : mCoordinator(coordinator)
        , mSquareModel(model)
    {
        assert(model != nullptr && "모델이 nullptr이면 안됨");
        assert(width > height && " WIDTH 가 더 커야 타일이 안찌그러짐");
        float tileScaleY = 2.0f / BOARD_SIZE;
        mTileScale = { tileScaleY * (static_cast<float>(height) / width), tileScaleY };

        mStartX = -(mTileScale.x * BOARD_SIZE / 2.0f) + (mTileScale.x / 2.0f);
        mStartY = -1.0f + (mTileScale.y / 2.0f);
        createTiles();
    }

    ~Board()
    {
        destroyBoard();
    }

    // 특정 칸의 월드 좌표 가져오기
    glm::vec2 getTilePosition(int col, int row) const {
        return { mStartX + (col * mTileScale.x), mStartY + (row * mTileScale.y) };
    }

    // 보드 전체 삭제 (필요 시)
    void destroyBoard() {
        for (auto entity : mTileEntities) {
            mCoordinator.DestroyEntity(entity);
        }
        mTileEntities.clear();
    }

    glm::vec2 GetTileScale()
    {
        return mTileScale;
    }

    constexpr float GetBoardSize()
    {
        return BOARD_SIZE;
    }

private:
    void createTiles() {
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                lve::Entity tile = mCoordinator.CreateEntity();

                mCoordinator.AddComponent(tile, lve::Transform2dComponent{
                    getTilePosition(col, row),
                    mTileScale,
                    0.f
                    });

                glm::vec3 color = ((row + col) % 2 == 0) ? lve::hexToColor(0xE6E6E6) : lve::hexToColor(0xB6876B);
                mCoordinator.AddComponent(tile, lve::RenderComponent{ mSquareModel, color });

                mTileEntities.push_back(tile);
            }
        }
    }

    lve::Coordinator& mCoordinator;
    std::vector<lve::Entity> mTileEntities; // 생성된 타일들을 관리
    glm::vec2 mTileScale;
    std::shared_ptr<lve::LveModel> mSquareModel;
    float mStartX, mStartY;
};