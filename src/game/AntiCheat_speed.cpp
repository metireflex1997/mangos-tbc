#include "AntiCheat_speed.h"
#include "CPlayer.h"
#include "World.h"
#include "Map.h"
#include "MapManager.h"

AntiCheat_speed::AntiCheat_speed(CPlayer* player) : AntiCheat(player)
{
    m_LastCheat = false;
}

bool AntiCheat_speed::HandleMovement(MovementInfo& moveInfo, Opcodes opcode)
{
    m_MoveInfo[0] = moveInfo; // moveInfo shouldn't be used anymore then assigning it in the beginning.

    bool skipcheat = false;

    if (!Initialized())
    {
        m_MoveInfo[1] = m_MoveInfo[0];
        m_MoveInfo[2] = m_MoveInfo[0];

        knockback_angle = 0.f;
        knockback_horizontalSpeed = 0.f;
        knockback_verticalSpeed = 0.f;
        return false;
    }

    if (opcode == MSG_MOVE_FALL_LAND)
    {
        knockback_angle = 0.f;
        knockback_horizontalSpeed = 0.f;
        knockback_verticalSpeed = 0.f;
    }

	if (GetDiff() < 50)
		return false;

    bool onTransport = isTransport(m_MoveInfo[0]) && isTransport(m_MoveInfo[1]);

    float allowedspeed = GetSpeed();

    float d2speed = (onTransport ? GetTransportDist2D() : GetDistance2D()) / GetVirtualDiffInSec();
    float d3speed = (onTransport ? GetTransportDist3D() : GetDistance3D()) / GetVirtualDiffInSec();
    float travelspeed = floor((isFlying() || isSwimming() ? d3speed : d2speed) * 100.f) / 100.f;

	bool cheating = false;

	if (travelspeed > allowedspeed && !isFalling())
		cheating = true;


	if (m_MoveInfo[0].GetJumpInfo().xyspeed > allowedspeed &&
        m_MoveInfo[0].GetJumpInfo().xyspeed != knockback_horizontalSpeed &&
        m_MoveInfo[0].GetJumpInfo().velocity != knockback_verticalSpeed)
		cheating = true;

    if (isTransport(m_MoveInfo[0]) && !verifyTransportCoords(m_MoveInfo[0]))
        cheating = false;

    if (cheating)
    {
        const Position* p = m_MoveInfo[2].GetPos();

        m_Player->TeleportTo(m_Player->GetMapId(), p->x, p->y, p->z, p->o, TELE_TO_NOT_LEAVE_COMBAT);

        m_Player->BoxChat << "SPEEDCHEAT" << "\n";
    }
    else
        m_MoveInfo[2] = m_MoveInfo[0];

    m_MoveInfo[1] = m_MoveInfo[0];

    return false;
}

void AntiCheat_speed::HandleKnockBack(float angle, float horizontalSpeed, float verticalSpeed)
{
    knockback_angle = angle;
    knockback_horizontalSpeed = horizontalSpeed;
    knockback_verticalSpeed = verticalSpeed;
}