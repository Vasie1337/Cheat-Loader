#include "esp.h"

void Esp::ActorLoop() {
	LocalPtrs::Gworld = read<uint64_t>(base + 0xEEDBD10); //https://fn.dumps.host/offsets?offset=GWorld
	if (!LocalPtrs::Gworld) return;

	uintptr_t GameInstance = read<uint64_t>(LocalPtrs::Gworld + 0x1b8); //https://fn.dumps.host/?class=UWorld&member=OwningGameInstance
	if (!GameInstance) return;

	LocalPtrs::LocalPlayers = read<uint64_t>(read<uint64_t>(GameInstance + 0x38)); //https://fn.dumps.host/?class=UGameInstance&member=LocalPlayers
	if (!LocalPtrs::LocalPlayers) return;

	LocalPtrs::PlayerController = read<uint64_t>(LocalPtrs::LocalPlayers + 0x30); //https://fn.dumps.host/?class=UPlayer&member=PlayerController
	if (!LocalPtrs::PlayerController) return;

	LocalPtrs::Player = read<uint64_t>(LocalPtrs::PlayerController + 0x330); //https://fn.dumps.host/?class=APlayerController&member=AcknowledgedPawn
	if (!LocalPtrs::Player) return;

	LocalPtrs::RootComponent = read<uint64_t>(LocalPtrs::Player + 0x190); //https://fn.dumps.host/?class=AActor&member=RootComponent
	if (!LocalPtrs::RootComponent) return;

	uintptr_t GameState = read<uintptr_t>(LocalPtrs::Gworld + 0x158); //https://fn.dumps.host/?class=UWorld&member=GameState
	if (!GameState) return;

	uintptr_t PlayerArrayOffset = 0x2a0; //https://fn.dumps.host/?class=AGameStateBase&member=PlayerArray

	uintptr_t PlayerArray = read<uintptr_t>(GameState + PlayerArrayOffset);

	int Num = read<int>(GameState + (PlayerArrayOffset + sizeof(uintptr_t)));

	for (int i = 0; i < Num; i++) {

		uintptr_t PlayerState = read<uintptr_t>(PlayerArray + (i * sizeof(uintptr_t)));
		if (!PlayerState) continue;

		uintptr_t Player = read<uintptr_t>(PlayerState + 0x300); //https://fn.dumps.host/?class=APlayerState&member=PawnPrivate
		if (!Player) continue;
		if (Player == LocalPtrs::Player) continue;

		uintptr_t Mesh = read<uintptr_t>(Player + 0x310); //https://fn.dumps.host/?class=ACharacter&member=Mesh
		if (!Mesh) continue;

		Vector3 Head3D = SDK::GetBoneWithRotation(Mesh, 68);
		Vector2 Head2D = SDK::ProjectWorldToScreen(Head3D);

		Vector3 Bottom3D = SDK::GetBoneWithRotation(Mesh, 0);
		Vector2 Bottom2D = SDK::ProjectWorldToScreen(Bottom3D);

		float BoxHeight = (float)(Head2D.y - Bottom2D.y);
		float CornerHeight = abs(Head2D.y - Bottom2D.y);
		float CornerWidth = BoxHeight * 0.80;

		Vector2 ScreenTop = { (double)Width / 2 , (double)Height };
		Vector2 ScreenCentre = { (double)Width / 2 , (double)Height / 2 };

		if (bCornerBox)
			Util::DrawBox(Head2D.x - (CornerWidth / 2), Head2D.y + (CornerWidth / 4), CornerWidth, CornerHeight - (CornerWidth / 4), ImColor(51, 214, 255, 160), 2.5f);

		if (bSnapLines)
			Util::DrawLine(ImVec2(ScreenCentre.x, ScreenCentre.y), ImVec2(Head2D.x, Head2D.y + (CornerWidth / 4)), ImColor(51, 214, 255, 160), 2.5f);
	}
}
