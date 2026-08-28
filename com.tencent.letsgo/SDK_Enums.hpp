// 创作者: 曦曦(DreamFekk) https://github.com/DreamFekk
// 禁止圈钱盗卖

#pragma once

#include <cstdio>
#include <string>
#include <cstdint>


// Package: CoreUObject
// Enums: 16

// Object: Enum CoreUObject.EInterpCurveMode
enum class EInterpCurveMode : uint8_t
{
	CIM_Linear = 0,
	CIM_CurveAuto = 1,
	CIM_Constant = 2,
	CIM_CurveUser = 3,
	CIM_CurveBreak = 4,
	CIM_CurveAutoClamped = 5,
	CIM_MAX = 6
};

// Object: Enum CoreUObject.ERangeBoundTypes
enum class ERangeBoundTypes : uint8_t
{
	Exclusive = 0,
	Inclusive = 1,
	Open = 2,
	ERangeBoundTypes_MAX = 3
};

// Object: Enum CoreUObject.ELocalizedTextSourceCategory
enum class ELocalizedTextSourceCategory : uint8_t
{
	Game = 0,
	Engine = 1,
	Editor = 2,
	ELocalizedTextSourceCategory_MAX = 3
};

// Object: Enum CoreUObject.EAutomationEventType
enum class EAutomationEventType : uint8_t
{
	Info = 0,
	Warning = 1,
	Error = 2,
	EAutomationEventType_MAX = 3
};

// Object: Enum CoreUObject.EMouseCursor
enum class EMouseCursor : uint8_t
{
	None = 0,
	Default = 1,
	TextEditBeam = 2,
	ResizeLeftRight = 3,
	ResizeUpDown = 4,
	ResizeSouthEast = 5,
	ResizeSouthWest = 6,
	CardinalCross = 7,
	Crosshairs = 8,
	Hand = 9,
	GrabHand = 10,
	GrabHandClosed = 11,
	SlashedCircle = 12,
	EyeDropper = 13,
	EMouseCursor_MAX = 14
};

// Object: Enum CoreUObject.ELifetimeCondition
enum class ELifetimeCondition : uint8_t
{
	COND_None = 0,
	COND_InitialOnly = 1,
	COND_OwnerOnly = 2,
	COND_SkipOwner = 3,
	COND_SimulatedOnly = 4,
	COND_AutonomousOnly = 5,
	COND_SimulatedOrPhysics = 6,
	COND_InitialOrOwner = 7,
	COND_Custom = 8,
	COND_ReplayOrOwner = 9,
	COND_ReplayOnly = 10,
	COND_SimulatedOnlyNoReplay = 11,
	COND_SimulatedOrPhysicsNoReplay = 12,
	COND_SkipReplay = 13,
	COND_Never = 15,
	COND_Max = 16
};

// Object: Enum CoreUObject.EDataValidationResult
enum class EDataValidationResult : uint8_t
{
	Invalid = 0,
	Valid = 1,
	NotValidated = 2,
	EDataValidationResult_MAX = 3
};

// Object: Enum CoreUObject.EAppMsgType
enum class EAppMsgType : uint8_t
{
	Ok = 0,
	YesNo = 1,
	OkCancel = 2,
	YesNoCancel = 3,
	CancelRetryContinue = 4,
	YesNoYesAllNoAll = 5,
	YesNoYesAllNoAllCancel = 6,
	YesNoYesAll = 7,
	EAppMsgType_MAX = 8
};

// Object: Enum CoreUObject.EAppReturnType
enum class EAppReturnType : uint8_t
{
	No = 0,
	Yes = 1,
	YesAll = 2,
	NoAll = 3,
	Cancel = 4,
	Ok = 5,
	Retry = 6,
	Continue = 7,
	EAppReturnType_MAX = 8
};

// Object: Enum CoreUObject.EPropertyAccessChangeNotifyMode
enum class EPropertyAccessChangeNotifyMode : uint8_t
{
	Default = 0,
	Never = 1,
	Always = 2,
	EPropertyAccessChangeNotifyMode_MAX = 3
};

// Object: Enum CoreUObject.EUnit
enum class EUnit : uint8_t
{
	Micrometers = 0,
	Millimeters = 1,
	Centimeters = 2,
	Meters = 3,
	Kilometers = 4,
	Inches = 5,
	Feet = 6,
	Yards = 7,
	Miles = 8,
	Lightyears = 9,
	Degrees = 10,
	Radians = 11,
	MetersPerSecond = 12,
	KilometersPerHour = 13,
	MilesPerHour = 14,
	Celsius = 15,
	Farenheit = 16,
	Kelvin = 17,
	Micrograms = 18,
	Milligrams = 19,
	Grams = 20,
	Kilograms = 21,
	MetricTons = 22,
	Ounces = 23,
	Pounds = 24,
	Stones = 25,
	Newtons = 26,
	PoundsForce = 27,
	KilogramsForce = 28,
	Hertz = 29,
	Kilohertz = 30,
	Megahertz = 31,
	Gigahertz = 32,
	RevolutionsPerMinute = 33,
	Bytes = 34,
	Kilobytes = 35,
	Megabytes = 36,
	Gigabytes = 37,
	Terabytes = 38,
	Lumens = 39,
	Milliseconds = 40,
	Seconds = 41,
	Minutes = 42,
	Hours = 43,
	Days = 44,
	Months = 45,
	Years = 46,
	Multiplier = 47,
	Percentage = 48,
	Unspecified = 49,
	EUnit_MAX = 50
};

// Object: Enum CoreUObject.EPixelFormat
enum class EPixelFormat : uint8_t
{
	PF_Unknown = 0,
	PF_A32B32G32R32F = 1,
	PF_B8G8R8A8 = 2,
	PF_G8 = 3,
	PF_G16 = 4,
	PF_DXT1 = 5,
	PF_DXT3 = 6,
	PF_DXT5 = 7,
	PF_UYVY = 8,
	PF_FloatRGB = 9,
	PF_FloatRGBA = 10,
	PF_DepthStencil = 11,
	PF_ShadowDepth = 12,
	PF_R32_FLOAT = 13,
	PF_G16R16 = 14,
	PF_G16R16F = 15,
	PF_G16R16F_FILTER = 16,
	PF_G32R32F = 17,
	PF_A2B10G10R10 = 18,
	PF_A16B16G16R16 = 19,
	PF_D24 = 20,
	PF_R16F = 21,
	PF_R16F_FILTER = 22,
	PF_BC5 = 23,
	PF_V8U8 = 24,
	PF_A1 = 25,
	PF_FloatR11G11B10 = 26,
	PF_A8 = 27,
	PF_R32_UINT = 28,
	PF_R32_SINT = 29,
	PF_PVRTC2 = 30,
	PF_PVRTC4 = 31,
	PF_R16_UINT = 32,
	PF_R16_SINT = 33,
	PF_R16G16B16A16_UINT = 34,
	PF_R16G16B16A16_SINT = 35,
	PF_R5G6B5_UNORM = 36,
	PF_R8G8B8A8 = 37,
	PF_A8R8G8B8 = 38,
	PF_BC4 = 39,
	PF_R8G8 = 40,
	PF_ATC_RGB = 41,
	PF_ATC_RGBA_E = 42,
	PF_ATC_RGBA_I = 43,
	PF_X24_G8 = 44,
	PF_ETC1 = 45,
	PF_ETC2_RGB = 46,
	PF_ETC2_RGBA = 47,
	PF_R32G32B32A32_UINT = 48,
	PF_R16G16_UINT = 49,
	PF_ASTC_4x4 = 50,
	PF_ASTC_6x6 = 51,
	PF_ASTC_8x8 = 52,
	PF_ASTC_10x10 = 53,
	PF_ASTC_12x12 = 54,
	PF_BC6H = 55,
	PF_BC7 = 56,
	PF_R8_UINT = 57,
	PF_L8 = 58,
	PF_XGXR8 = 59,
	PF_R8G8B8A8_UINT = 60,
	PF_R8G8B8A8_SNORM = 61,
	PF_R16G16B16A16_UNORM = 62,
	PF_R16G16B16A16_SNORM = 63,
	PF_PLATFORM_HDR = 64,
	PF_PLATFORM_HDR = 65,
	PF_PLATFORM_HDR = 66,
	PF_NV12 = 67,
	PF_R32G32_UINT = 68,
	PF_ETC2_R11_EAC = 69,
	PF_ETC2_RG11_EAC = 70,
	PF_MAX = 72
};

// Object: Enum CoreUObject.EAxis
enum class EAxis : uint8_t
{
	None = 0,
	X = 1,
	Y = 2,
	Z = 3,
	EAxis_MAX = 4
};

// Object: Enum CoreUObject.ELogTimes
enum class ELogTimes : uint8_t
{
	None = 0,
	UTC = 1,
	SinceGStartTime = 2,
	Local = 3,
	ELogTimes_MAX = 4
};

// Object: Enum CoreUObject.ESearchDir
enum class ESearchDir : uint8_t
{
	FromStart = 0,
	FromEnd = 1,
	ESearchDir_MAX = 2
};

// Object: Enum CoreUObject.ESearchCase
enum class ESearchCase : uint8_t
{
	CaseSensitive = 0,
	IgnoreCase = 1,
	ESearchCase_MAX = 2
};

// Package: Engine
// Enums: 457

// Object: Enum Engine.ETextGender
enum class ETextGender : uint8_t
{
	Masculine = 0,
	Feminine = 1,
	Neuter = 2,
	ETextGender_MAX = 3
};

// Object: Enum Engine.EFormatArgumentType
enum class EFormatArgumentType : uint8_t
{
	Int = 0,
	UInt = 1,
	Float = 2,
	Double = 3,
	Text = 4,
	Gender = 5,
	EFormatArgumentType_MAX = 6
};

// Object: Enum Engine.EEndPlayReason
enum class EEndPlayReason : uint8_t
{
	Destroyed = 0,
	LevelTransition = 1,
	EndPlayInEditor = 2,
	RemovedFromWorld = 3,
	Quit = 4,
	EEndPlayReason_MAX = 5
};

// Object: Enum Engine.ENetworkFailure
enum class ENetworkFailure : uint8_t
{
	NetDriverAlreadyExists = 0,
	NetDriverCreateFailure = 1,
	NetDriverListenFailure = 2,
	ConnectionLost = 3,
	ConnectionTimeout = 4,
	FailureReceived = 5,
	OutdatedClient = 6,
	OutdatedServer = 7,
	PendingConnectionFailure = 8,
	NetGuidMismatch = 9,
	NetChecksumMismatch = 10,
	ENetworkFailure_MAX = 11
};

// Object: Enum Engine.ETravelFailure
enum class ETravelFailure : uint8_t
{
	NoLevel = 0,
	LoadMapFailure = 1,
	InvalidURL = 2,
	PackageMissing = 3,
	PackageVersion = 4,
	NoDownload = 5,
	TravelFailure = 6,
	CheatCommands = 7,
	PendingNetGameCreateFailure = 8,
	CloudSaveFailure = 9,
	ServerTravelFailure = 10,
	ClientTravelFailure = 11,
	ETravelFailure_MAX = 12
};

// Object: Enum Engine.ETickingGroup
enum class ETickingGroup : uint8_t
{
	TG_PrePhysics = 0,
	TG_StartPhysics = 1,
	TG_DuringPhysics = 2,
	TG_EndPhysics = 3,
	TG_PostPhysics = 4,
	TG_PostUpdateWork = 5,
	TG_LastDemotable = 6,
	TG_NewlySpawned = 7,
	TG_MAX = 8
};

// Object: Enum Engine.EComponentCreationMethod
enum class EComponentCreationMethod : uint8_t
{
	Native = 0,
	SimpleConstructionScript = 1,
	UserConstructionScript = 2,
	Instance = 3,
	EComponentCreationMethod_MAX = 4
};

// Object: Enum Engine.ETemperatureSeverityType
enum class ETemperatureSeverityType : uint8_t
{
	Unknown = 0,
	Good = 1,
	Bad = 2,
	Serious = 3,
	Critical = 4,
	NumSeverities = 5,
	ETemperatureSeverityType_MAX = 6
};

// Object: Enum Engine.EQuartzCommandQuantization
enum class EQuartzCommandQuantization : uint8_t
{
	Bar = 0,
	Beat = 1,
	ThirtySecondNote = 2,
	SixteenthNote = 3,
	EighthNote = 4,
	QuarterNote = 5,
	HalfNote = 6,
	WholeNote = 7,
	DottedSixteenthNote = 8,
	DottedEighthNote = 9,
	DottedQuarterNote = 10,
	DottedHalfNote = 11,
	DottedWholeNote = 12,
	SixteenthNoteTriplet = 13,
	EighthNoteTriplet = 14,
	QuarterNoteTriplet = 15,
	HalfNoteTriplet = 16,
	Tick = 17,
	Count = 18,
	EQuartzCommandQuantization_MAX = 19
};

// Object: Enum Engine.EQuartzCommandDelegateSubType
enum class EQuartzCommandDelegateSubType : uint8_t
{
	CommandOnFailedToQueue = 0,
	CommandOnQueued = 1,
	CommandOnCanceled = 2,
	CommandOnAboutToStart = 3,
	CommandOnStarted = 4,
	Count = 5,
	EQuartzCommandDelegateSubType_MAX = 6
};

// Object: Enum Engine.EAudioComponentPlayState
enum class EAudioComponentPlayState : uint8_t
{
	Playing = 0,
	Stopped = 1,
	Paused = 2,
	FadingIn = 3,
	FadingOut = 4,
	Count = 5,
	EAudioComponentPlayState_MAX = 6
};

// Object: Enum Engine.EPlaneConstraintAxisSetting
enum class EPlaneConstraintAxisSetting : uint8_t
{
	Custom = 0,
	X = 1,
	Y = 2,
	Z = 3,
	UseGlobalPhysicsSetting = 4,
	EPlaneConstraintAxisSetting_MAX = 5
};

// Object: Enum Engine.EInterpToBehaviourType
enum class EInterpToBehaviourType : uint8_t
{
	OneShot = 0,
	OneShot_Reverse = 1,
	Loop_Reset = 2,
	PingPong = 3,
	EInterpToBehaviourType_MAX = 4
};

// Object: Enum Engine.ETeleportType
enum class ETeleportType : uint8_t
{
	None = 0,
	TeleportPhysics = 1,
	ResetPhysics = 2,
	ETeleportType_MAX = 3
};

// Object: Enum Engine.EPlatformInterfaceDataType
enum class EPlatformInterfaceDataType : uint8_t
{
	PIDT_None = 0,
	PIDT_Int = 1,
	PIDT_Float = 2,
	PIDT_String = 3,
	PIDT_Object = 4,
	PIDT_Custom = 5,
	PIDT_MAX = 6
};

// Object: Enum Engine.EMovementMode
enum class EMovementMode : uint8_t
{
	MOVE_None = 0,
	MOVE_Walking = 1,
	MOVE_NavWalking = 2,
	MOVE_Falling = 3,
	MOVE_Swimming = 4,
	MOVE_Flying = 5,
	MOVE_Custom = 6,
	MOVE_MAX = 7
};

// Object: Enum Engine.EScreenOrientation
enum class EScreenOrientation : uint8_t
{
	Unknown = 0,
	Portrait = 1,
	PortraitUpsideDown = 2,
	LandscapeLeft = 3,
	LandscapeRight = 4,
	FaceUp = 5,
	FaceDown = 6,
	EScreenOrientation_MAX = 7
};

// Object: Enum Engine.EApplicationState
enum class EApplicationState : uint8_t
{
	Unknown = 0,
	Inactive = 1,
	Background = 2,
	Active = 3,
	EApplicationState_MAX = 4
};

// Object: Enum Engine.EObjectTypeQuery
enum class EObjectTypeQuery : uint8_t
{
	ObjectTypeQuery1 = 0,
	ObjectTypeQuery2 = 1,
	ObjectTypeQuery3 = 2,
	ObjectTypeQuery4 = 3,
	ObjectTypeQuery5 = 4,
	ObjectTypeQuery6 = 5,
	ObjectTypeQuery7 = 6,
	ObjectTypeQuery8 = 7,
	ObjectTypeQuery9 = 8,
	ObjectTypeQuery10 = 9,
	ObjectTypeQuery11 = 10,
	ObjectTypeQuery12 = 11,
	ObjectTypeQuery13 = 12,
	ObjectTypeQuery14 = 13,
	ObjectTypeQuery15 = 14,
	ObjectTypeQuery16 = 15,
	ObjectTypeQuery17 = 16,
	ObjectTypeQuery18 = 17,
	ObjectTypeQuery19 = 18,
	ObjectTypeQuery20 = 19,
	ObjectTypeQuery21 = 20,
	ObjectTypeQuery22 = 21,
	ObjectTypeQuery23 = 22,
	ObjectTypeQuery24 = 23,
	ObjectTypeQuery25 = 24,
	ObjectTypeQuery26 = 25,
	ObjectTypeQuery27 = 26,
	ObjectTypeQuery28 = 27,
	ObjectTypeQuery29 = 28,
	ObjectTypeQuery30 = 29,
	ObjectTypeQuery31 = 30,
	ObjectTypeQuery32 = 31,
	ObjectTypeQuery33 = 32,
	ObjectTypeQuery34 = 33,
	ObjectTypeQuery35 = 34,
	ObjectTypeQuery36 = 35,
	ObjectTypeQuery37 = 36,
	ObjectTypeQuery38 = 37,
	ObjectTypeQuery39 = 38,
	ObjectTypeQuery40 = 39,
	ObjectTypeQuery41 = 40,
	ObjectTypeQuery42 = 41,
	ObjectTypeQuery43 = 42,
	ObjectTypeQuery44 = 43,
	ObjectTypeQuery45 = 44,
	ObjectTypeQuery46 = 45,
	ObjectTypeQuery47 = 46,
	ObjectTypeQuery48 = 47,
	ObjectTypeQuery49 = 48,
	ObjectTypeQuery50 = 49,
	ObjectTypeQuery51 = 50,
	ObjectTypeQuery52 = 51,
	ObjectTypeQuery53 = 52,
	ObjectTypeQuery54 = 53,
	ObjectTypeQuery55 = 54,
	ObjectTypeQuery56 = 55,
	ObjectTypeQuery57 = 56,
	ObjectTypeQuery58 = 57,
	ObjectTypeQuery59 = 58,
	ObjectTypeQuery60 = 59,
	ObjectTypeQuery61 = 60,
	ObjectTypeQuery62 = 61,
	ObjectTypeQuery63 = 62,
	ObjectTypeQuery64 = 63,
	ObjectTypeQuery_MAX = 64,
	EObjectTypeQuery_MAX = 65
};

// Object: Enum Engine.EDrawDebugTrace
enum class EDrawDebugTrace : uint8_t
{
	None = 0,
	ForOneFrame = 1,
	ForDuration = 2,
	Persistent = 3,
	EDrawDebugTrace_MAX = 4
};

// Object: Enum Engine.ETraceTypeQuery
enum class ETraceTypeQuery : uint8_t
{
	TraceTypeQuery1 = 0,
	TraceTypeQuery2 = 1,
	TraceTypeQuery3 = 2,
	TraceTypeQuery4 = 3,
	TraceTypeQuery5 = 4,
	TraceTypeQuery6 = 5,
	TraceTypeQuery7 = 6,
	TraceTypeQuery8 = 7,
	TraceTypeQuery9 = 8,
	TraceTypeQuery10 = 9,
	TraceTypeQuery11 = 10,
	TraceTypeQuery12 = 11,
	TraceTypeQuery13 = 12,
	TraceTypeQuery14 = 13,
	TraceTypeQuery15 = 14,
	TraceTypeQuery16 = 15,
	TraceTypeQuery17 = 16,
	TraceTypeQuery18 = 17,
	TraceTypeQuery19 = 18,
	TraceTypeQuery20 = 19,
	TraceTypeQuery21 = 20,
	TraceTypeQuery22 = 21,
	TraceTypeQuery23 = 22,
	TraceTypeQuery24 = 23,
	TraceTypeQuery25 = 24,
	TraceTypeQuery26 = 25,
	TraceTypeQuery27 = 26,
	TraceTypeQuery28 = 27,
	TraceTypeQuery29 = 28,
	TraceTypeQuery30 = 29,
	TraceTypeQuery31 = 30,
	TraceTypeQuery32 = 31,
	TraceTypeQuery33 = 32,
	TraceTypeQuery34 = 33,
	TraceTypeQuery35 = 34,
	TraceTypeQuery36 = 35,
	TraceTypeQuery37 = 36,
	TraceTypeQuery38 = 37,
	TraceTypeQuery39 = 38,
	TraceTypeQuery40 = 39,
	TraceTypeQuery41 = 40,
	TraceTypeQuery42 = 41,
	TraceTypeQuery43 = 42,
	TraceTypeQuery44 = 43,
	TraceTypeQuery45 = 44,
	TraceTypeQuery46 = 45,
	TraceTypeQuery47 = 46,
	TraceTypeQuery48 = 47,
	TraceTypeQuery49 = 48,
	TraceTypeQuery50 = 49,
	TraceTypeQuery51 = 50,
	TraceTypeQuery52 = 51,
	TraceTypeQuery53 = 52,
	TraceTypeQuery54 = 53,
	TraceTypeQuery55 = 54,
	TraceTypeQuery56 = 55,
	TraceTypeQuery57 = 56,
	TraceTypeQuery58 = 57,
	TraceTypeQuery59 = 58,
	TraceTypeQuery60 = 59,
	TraceTypeQuery61 = 60,
	TraceTypeQuery62 = 61,
	TraceTypeQuery63 = 62,
	TraceTypeQuery64 = 63,
	TraceTypeQuery_MAX = 64,
	ETraceTypeQuery_MAX = 65
};

// Object: Enum Engine.EMoveComponentAction
enum class EMoveComponentAction : uint8_t
{
	Move = 0,
	Stop = 1,
	Return = 2,
	EMoveComponentAction_MAX = 3
};

// Object: Enum Engine.EQuitPreference
enum class EQuitPreference : uint8_t
{
	Quit = 0,
	Background = 1,
	EQuitPreference_MAX = 2
};

// Object: Enum Engine.ERichCurveExtrapolation
enum class ERichCurveExtrapolation : uint8_t
{
	RCCE_Cycle = 0,
	RCCE_CycleWithOffset = 1,
	RCCE_Oscillate = 2,
	RCCE_Linear = 3,
	RCCE_Constant = 4,
	RCCE_None = 5,
	RCCE_MAX = 6
};

// Object: Enum Engine.ERichCurveTangentWeightMode
enum class ERichCurveTangentWeightMode : uint8_t
{
	RCTWM_WeightedNone = 0,
	RCTWM_WeightedArrive = 1,
	RCTWM_WeightedLeave = 2,
	RCTWM_WeightedBoth = 3,
	RCTWM_MAX = 4
};

// Object: Enum Engine.ERichCurveTangentMode
enum class ERichCurveTangentMode : uint8_t
{
	RCTM_Auto = 0,
	RCTM_User = 1,
	RCTM_Break = 2,
	RCTM_None = 3,
	RCTM_MAX = 4
};

// Object: Enum Engine.ERichCurveInterpMode
enum class ERichCurveInterpMode : uint8_t
{
	RCIM_Linear = 0,
	RCIM_Constant = 1,
	RCIM_Cubic = 2,
	RCIM_None = 3,
	RCIM_MAX = 4
};

// Object: Enum Engine.ESignalType
enum class ESignalType : uint8_t
{
	EST_EnableCustomOptimize = 0,
	EST_DisableCustomOptimize = 1,
	EST_EnableTickFrequencyOptimize = 2,
	EST_DisableTickFrequencyOptimize = 3,
	EST_SignalType5 = 4,
	EST_SignalType6 = 5,
	EST_SignalType7 = 6,
	EST_SignalType8 = 7,
	EST_SignalType9 = 8,
	EST_SignalType10 = 9,
	EST_SignalType11 = 10,
	EST_SignalType12 = 11,
	EST_SignalType13 = 12,
	EST_SignalType14 = 13,
	EST_SignalType15 = 14,
	EST_SignalType16 = 15,
	EST_SignalType17 = 16,
	EST_SignalType18 = 17,
	EST_SignalType19 = 18,
	EST_SignalType20 = 19,
	EST_MAX = 20
};

// Object: Enum Engine.ENetRole
enum class ENetRole : uint8_t
{
	ROLE_None = 0,
	ROLE_SimulatedProxy = 1,
	ROLE_AutonomousProxy = 2,
	ROLE_Authority = 3,
	ROLE_MAX = 4
};

// Object: Enum Engine.EAttachLocation
enum class EAttachLocation : uint8_t
{
	KeepRelativeOffset = 0,
	KeepWorldPosition = 1,
	SnapToTarget = 2,
	SnapToTargetIncludingScale = 3,
	EAttachLocation_MAX = 4
};

// Object: Enum Engine.EAttachmentRule
enum class EAttachmentRule : uint8_t
{
	KeepRelative = 0,
	KeepWorld = 1,
	SnapToTarget = 2,
	EAttachmentRule_MAX = 3
};

// Object: Enum Engine.EDetachmentRule
enum class EDetachmentRule : uint8_t
{
	KeepRelative = 0,
	KeepWorld = 1,
	EDetachmentRule_MAX = 2
};

// Object: Enum Engine.ENetDormancy
enum class ENetDormancy : uint8_t
{
	DORM_Never = 0,
	DORM_Awake = 1,
	DORM_DormantAll = 2,
	DORM_DormantPartial = 3,
	DORM_Initial = 4,
	DORM_MAX = 5
};

// Object: Enum Engine.EAutoReceiveInput
enum class EAutoReceiveInput : uint8_t
{
	Disabled = 0,
	Player0 = 1,
	Player1 = 2,
	Player2 = 3,
	Player3 = 4,
	Player4 = 5,
	Player5 = 6,
	Player6 = 7,
	Player7 = 8,
	EAutoReceiveInput_MAX = 9
};

// Object: Enum Engine.ESpawnActorCollisionHandlingMethod
enum class ESpawnActorCollisionHandlingMethod : uint8_t
{
	Undefined = 0,
	AlwaysSpawn = 1,
	AdjustIfPossibleButAlwaysSpawn = 2,
	AdjustIfPossibleButDontSpawnIfColliding = 3,
	DontSpawnIfColliding = 4,
	ESpawnActorCollisionHandlingMethod_MAX = 5
};

// Object: Enum Engine.ERotatorQuantization
enum class ERotatorQuantization : uint8_t
{
	ByteComponents = 0,
	ShortComponents = 1,
	ERotatorQuantization_MAX = 2
};

// Object: Enum Engine.EVectorQuantization
enum class EVectorQuantization : uint8_t
{
	RoundWholeNumber = 0,
	RoundOneDecimal = 1,
	RoundTwoDecimals = 2,
	EVectorQuantization_MAX = 3
};

// Object: Enum Engine.EActorUpdateOverlapsMethod
enum class EActorUpdateOverlapsMethod : uint8_t
{
	UseConfigDefault = 0,
	AlwaysUpdate = 1,
	OnlyUpdateMovable = 2,
	NeverUpdate = 3,
	EActorUpdateOverlapsMethod_MAX = 4
};

// Object: Enum Engine.EAutoPossessAI
enum class EAutoPossessAI : uint8_t
{
	Disabled = 0,
	PlacedInWorld = 1,
	Spawned = 2,
	PlacedInWorldOrSpawned = 3,
	EAutoPossessAI_MAX = 4
};

// Object: Enum Engine.ENetworkSmoothingMode
enum class ENetworkSmoothingMode : uint8_t
{
	Disabled = 0,
	Linear = 1,
	Exponential = 2,
	Interpolation = 3,
	Replay = 4,
	ENetworkSmoothingMode_MAX = 5
};

// Object: Enum Engine.ECameraShakePlaySpace
enum class ECameraShakePlaySpace : uint8_t
{
	CameraLocal = 0,
	World = 1,
	UserDefined = 2,
	ECameraShakePlaySpace_MAX = 3
};

// Object: Enum Engine.EViewTargetBlendFunction
enum class EViewTargetBlendFunction : uint8_t
{
	VTBlend_Linear = 0,
	VTBlend_Cubic = 1,
	VTBlend_EaseIn = 2,
	VTBlend_EaseOut = 3,
	VTBlend_EaseInOut = 4,
	VTBlend_MAX = 5
};

// Object: Enum Engine.ETravelType
enum class ETravelType : uint8_t
{
	TRAVEL_Absolute = 0,
	TRAVEL_Partial = 1,
	TRAVEL_Relative = 2,
	TRAVEL_MAX = 3
};

// Object: Enum Engine.ECollisionChannel
enum class ECollisionChannel : uint8_t
{
	ECC_WorldStatic = 0,
	ECC_WorldDynamic = 1,
	ECC_Pawn = 2,
	ECC_Visibility = 3,
	ECC_Camera = 4,
	ECC_PhysicsBody = 5,
	ECC_Vehicle = 6,
	ECC_Destructible = 7,
	ECC_EngineTraceChannel1 = 8,
	ECC_EngineTraceChannel2 = 9,
	ECC_EngineTraceChannel3 = 10,
	ECC_EngineTraceChannel4 = 11,
	ECC_EngineTraceChannel5 = 12,
	ECC_EngineTraceChannel6 = 13,
	ECC_GameTraceChannel1 = 14,
	ECC_GameTraceChannel2 = 15,
	ECC_GameTraceChannel3 = 16,
	ECC_GameTraceChannel4 = 17,
	ECC_GameTraceChannel5 = 18,
	ECC_GameTraceChannel6 = 19,
	ECC_GameTraceChannel7 = 20,
	ECC_GameTraceChannel8 = 21,
	ECC_GameTraceChannel9 = 22,
	ECC_GameTraceChannel10 = 23,
	ECC_GameTraceChannel11 = 24,
	ECC_GameTraceChannel12 = 25,
	ECC_GameTraceChannel13 = 26,
	ECC_GameTraceChannel14 = 27,
	ECC_GameTraceChannel15 = 28,
	ECC_GameTraceChannel16 = 29,
	ECC_GameTraceChannel17 = 30,
	ECC_GameTraceChannel18 = 31,
	ECC_GameTraceChannel19 = 32,
	ECC_GameTraceChannel20 = 33,
	ECC_GameTraceChannel21 = 34,
	ECC_GameTraceChannel22 = 35,
	ECC_GameTraceChannel23 = 36,
	ECC_GameTraceChannel24 = 37,
	ECC_GameTraceChannel25 = 38,
	ECC_GameTraceChannel26 = 39,
	ECC_GameTraceChannel27 = 40,
	ECC_GameTraceChannel28 = 41,
	ECC_GameTraceChannel29 = 42,
	ECC_GameTraceChannel30 = 43,
	ECC_GameTraceChannel31 = 44,
	ECC_GameTraceChannel32 = 45,
	ECC_GameTraceChannel33 = 46,
	ECC_GameTraceChannel34 = 47,
	ECC_GameTraceChannel35 = 48,
	ECC_GameTraceChannel36 = 49,
	ECC_GameTraceChannel37 = 50,
	ECC_GameTraceChannel38 = 51,
	ECC_GameTraceChannel39 = 52,
	ECC_GameTraceChannel40 = 53,
	ECC_GameTraceChannel41 = 54,
	ECC_GameTraceChannel42 = 55,
	ECC_GameTraceChannel43 = 56,
	ECC_GameTraceChannel44 = 57,
	ECC_GameTraceChannel45 = 58,
	ECC_GameTraceChannel46 = 59,
	ECC_GameTraceChannel47 = 60,
	ECC_GameTraceChannel48 = 61,
	ECC_GameTraceChannel49 = 62,
	ECC_OverlapAll_Deprecated = 63,
	ECC_MAX = 64
};

// Object: Enum Engine.EControllerAnalogStick
enum class EControllerAnalogStick : uint8_t
{
	CAS_LeftStick = 0,
	CAS_RightStick = 1,
	CAS_MAX = 2
};

// Object: Enum Engine.EDynamicForceFeedbackAction
enum class EDynamicForceFeedbackAction : uint8_t
{
	Start = 0,
	Update = 1,
	Stop = 2,
	EDynamicForceFeedbackAction_MAX = 3
};

// Object: Enum Engine.EBoneSpaces
enum class EBoneSpaces : uint8_t
{
	WorldSpace = 0,
	ComponentSpace = 1,
	EBoneSpaces_MAX = 2
};

// Object: Enum Engine.ERelativeTransformSpace
enum class ERelativeTransformSpace : uint8_t
{
	RTS_World = 0,
	RTS_Actor = 1,
	RTS_Component = 2,
	RTS_ParentBoneSpace = 3,
	RTS_MAX = 4
};

// Object: Enum Engine.EComponentMobility
enum class EComponentMobility : uint8_t
{
	Static = 0,
	Stationary = 1,
	Movable = 2,
	EComponentMobility_MAX = 3
};

// Object: Enum Engine.EDetailMode
enum class EDetailMode : uint8_t
{
	DM_Low = 0,
	DM_Medium = 1,
	DM_High = 2,
	DM_MAX = 3
};

// Object: Enum Engine.EFastArraySerializerDeltaFlags
enum class EFastArraySerializerDeltaFlags : uint8_t
{
	None = 0,
	HasBeenSerialized = 1,
	HasDeltaBeenRequested = 2,
	IsUsingDeltaSerialization = 4,
	EFastArraySerializerDeltaFlags_MAX = 5
};

// Object: Enum Engine.EMouseLockMode
enum class EMouseLockMode : uint8_t
{
	DoNotLock = 0,
	LockOnCapture = 1,
	LockAlways = 2,
	LockInFullscreen = 3,
	EMouseLockMode_MAX = 4
};

// Object: Enum Engine.EWindowTitleBarMode
enum class EWindowTitleBarMode : uint8_t
{
	Overlay = 0,
	VerticalBox = 1,
	EWindowTitleBarMode_MAX = 2
};

// Object: Enum Engine.EInputEvent
enum class EInputEvent : uint8_t
{
	IE_Pressed = 0,
	IE_Released = 1,
	IE_Repeat = 2,
	IE_DoubleClick = 3,
	IE_Axis = 4,
	IE_MAX = 5
};

// Object: Enum Engine.ESpawnActorUgcMobility
enum class ESpawnActorUgcMobility : uint8_t
{
	Undefined = 0,
	UgcStatic = 1,
	UgcMobility = 2,
	ESpawnActorUgcMobility_MAX = 3
};

// Object: Enum Engine.EIndirectLightingCacheQuality
enum class EIndirectLightingCacheQuality : uint8_t
{
	ILCQ_Off = 0,
	ILCQ_Point = 1,
	ILCQ_Volume = 2,
	ILCQ_MAX = 3
};

// Object: Enum Engine.EOscillatorWaveform
enum class EOscillatorWaveform : uint8_t
{
	SineWave = 0,
	PerlinNoise = 1,
	EOscillatorWaveform_MAX = 2
};

// Object: Enum Engine.EInitialOscillatorOffset
enum class EInitialOscillatorOffset : uint8_t
{
	EOO_OffsetRandom = 0,
	EOO_OffsetZero = 1,
	EOO_MAX = 2
};

// Object: Enum Engine.EAcDecalScreenAlignment
enum class EAcDecalScreenAlignment : uint8_t
{
	PSDA_DecalFaceLockedAxis = 0,
	PSDA_MAX = 1
};

// Object: Enum Engine.EAlphaBlendOption
enum class EAlphaBlendOption : uint8_t
{
	Linear = 0,
	Cubic = 1,
	HermiteCubic = 2,
	Sinusoidal = 3,
	QuadraticInOut = 4,
	CubicInOut = 5,
	QuarticInOut = 6,
	QuinticInOut = 7,
	CircularIn = 8,
	CircularOut = 9,
	CircularInOut = 10,
	ExpIn = 11,
	ExpOut = 12,
	ExpInOut = 13,
	Custom = 14,
	EAlphaBlendOption_MAX = 15
};

// Object: Enum Engine.EAnimSyncGroupScope
enum class EAnimSyncGroupScope : uint8_t
{
	Local = 0,
	Component = 1,
	EAnimSyncGroupScope_MAX = 2
};

// Object: Enum Engine.EAnimGroupRole
enum class EAnimGroupRole : uint8_t
{
	CanBeLeader = 0,
	AlwaysFollower = 1,
	AlwaysLeader = 2,
	TransitionLeader = 3,
	TransitionFollower = 4,
	EAnimGroupRole_MAX = 5
};

// Object: Enum Engine.EPreviewAnimationBlueprintApplicationMethod
enum class EPreviewAnimationBlueprintApplicationMethod : uint8_t
{
	LinkedLayers = 0,
	LinkedAnimGraph = 1,
	EPreviewAnimationBlueprintApplicationMethod_MAX = 2
};

// Object: Enum Engine.AnimationKeyFormat
enum class EAnimationKeyFormat : uint8_t
{
	AKF_ConstantKeyLerp = 0,
	AKF_VariableKeyLerp = 1,
	AKF_PerTrackCompression = 2,
	AKF_MAX = 3
};

// Object: Enum Engine.ERawCurveTrackTypes
enum class ERawCurveTrackTypes : uint8_t
{
	RCT_Float = 0,
	RCT_Vector = 1,
	RCT_Transform = 2,
	RCT_MAX = 3
};

// Object: Enum Engine.EAnimAssetCurveFlags
enum class EAnimAssetCurveFlags : uint8_t
{
	AACF_NONE = 0,
	AACF_DriveMorphTarget_DEPRECATED = 1,
	AACF_DriveAttribute_DEPRECATED = 2,
	AACF_Editable = 4,
	AACF_DriveMaterial_DEPRECATED = 8,
	AACF_Metadata = 16,
	AACF_DriveTrack = 32,
	AACF_Disabled = 64,
	AACF_MAX = 65
};

// Object: Enum Engine.AnimationCompressionFormat
enum class EAnimationCompressionFormat : uint8_t
{
	ACF_None = 0,
	ACF_Float96NoW = 1,
	ACF_Fixed48NoW = 2,
	ACF_IntervalFixed32NoW = 3,
	ACF_Fixed32NoW = 4,
	ACF_Float32NoW = 5,
	ACF_Identity = 6,
	ACF_MAX = 7
};

// Object: Enum Engine.EAdditiveBasePoseType
enum class EAdditiveBasePoseType : uint8_t
{
	ABPT_None = 0,
	ABPT_RefPose = 1,
	ABPT_AnimScaled = 2,
	ABPT_AnimFrame = 3,
	ABPT_MAX = 4
};

// Object: Enum Engine.ERootMotionMode
enum class ERootMotionMode : uint8_t
{
	NoRootMotionExtraction = 0,
	IgnoreRootMotion = 1,
	RootMotionFromEverything = 2,
	RootMotionFromMontagesOnly = 3,
	ERootMotionMode_MAX = 4
};

// Object: Enum Engine.ERootMotionRootLock
enum class ERootMotionRootLock : uint8_t
{
	RefPose = 0,
	AnimFirstFrame = 1,
	Zero = 2,
	ERootMotionRootLock_MAX = 3
};

// Object: Enum Engine.EMontagePlayReturnType
enum class EMontagePlayReturnType : uint8_t
{
	MontageLength = 0,
	Duration = 1,
	EMontagePlayReturnType_MAX = 2
};

// Object: Enum Engine.EDrawDebugItemType
enum class EDrawDebugItemType : uint8_t
{
	DirectionalArrow = 0,
	Sphere = 1,
	Line = 2,
	OnScreenMessage = 3,
	CoordinateSystem = 4,
	EDrawDebugItemType_MAX = 5
};

// Object: Enum Engine.EAnimLinkMethod
enum class EAnimLinkMethod : uint8_t
{
	Absolute = 0,
	Relative = 1,
	Proportional = 2,
	EAnimLinkMethod_MAX = 3
};

// Object: Enum Engine.EMontageSubStepResult
enum class EMontageSubStepResult : uint8_t
{
	Moved = 0,
	NotMoved = 1,
	InvalidSection = 2,
	InvalidMontage = 3,
	EMontageSubStepResult_MAX = 4
};

// Object: Enum Engine.EMontageBlendMode
enum class EMontageBlendMode : uint8_t
{
	Standard = 0,
	Inertialization = 1,
	EMontageBlendMode_MAX = 2
};

// Object: Enum Engine.EAnimNotifyEventType
enum class EAnimNotifyEventType : uint8_t
{
	Begin = 0,
	End = 1,
	EAnimNotifyEventType_MAX = 2
};

// Object: Enum Engine.EInertializationSpace
enum class EInertializationSpace : uint8_t
{
	Default = 0,
	WorldSpace = 1,
	WorldRotation = 2,
	EInertializationSpace_MAX = 3
};

// Object: Enum Engine.EInertializationBoneState
enum class EInertializationBoneState : uint8_t
{
	Invalid = 0,
	Valid = 1,
	Excluded = 2,
	EInertializationBoneState_MAX = 3
};

// Object: Enum Engine.EInertializationState
enum class EInertializationState : uint8_t
{
	Inactive = 0,
	Pending = 1,
	Active = 2,
	EInertializationState_MAX = 3
};

// Object: Enum Engine.EEvaluatorMode
enum class EEvaluatorMode : uint8_t
{
	EM_Standard = 0,
	EM_Freeze = 1,
	EM_DelayedFreeze = 2,
	EM_MAX = 3
};

// Object: Enum Engine.EEvaluatorDataSource
enum class EEvaluatorDataSource : uint8_t
{
	EDS_SourcePose = 0,
	EDS_DestinationPose = 1,
	EDS_MAX = 2
};

// Object: Enum Engine.EPostCopyOperation
enum class EPostCopyOperation : uint8_t
{
	None = 0,
	LogicalNegateBool = 1,
	EPostCopyOperation_MAX = 2
};

// Object: Enum Engine.EPinHidingMode
enum class EPinHidingMode : uint8_t
{
	NeverAsPin = 0,
	PinHiddenByDefault = 1,
	PinShownByDefault = 2,
	AlwaysAsPin = 3,
	EPinHidingMode_MAX = 4
};

// Object: Enum Engine.AnimPhysCollisionType
enum class EAnimPhysCollisionType : uint8_t
{
	CoM = 0,
	CustomSphere = 1,
	InnerSphere = 2,
	OuterSphere = 3,
	AnimPhysCollisionType_MAX = 4
};

// Object: Enum Engine.AnimPhysTwistAxis
enum class EAnimPhysTwistAxis : uint8_t
{
	AxisX = 0,
	AxisY = 1,
	AxisZ = 2,
	AnimPhysTwistAxis_MAX = 3
};

// Object: Enum Engine.ETypeAdvanceAnim
enum class ETypeAdvanceAnim : uint8_t
{
	ETAA_Default = 0,
	ETAA_Finished = 1,
	ETAA_Looped = 2,
	ETAA_MAX = 3
};

// Object: Enum Engine.ETransitionLogicType
enum class ETransitionLogicType : uint8_t
{
	TLT_StandardBlend = 0,
	TLT_Inertialization = 1,
	TLT_Custom = 2,
	TLT_MAX = 3
};

// Object: Enum Engine.ETransitionBlendMode
enum class ETransitionBlendMode : uint8_t
{
	TBM_Linear = 0,
	TBM_Cubic = 1,
	TBM_MAX = 2
};

// Object: Enum Engine.EComponentType
enum class EComponentType : uint8_t
{
	None = 0,
	TranslationX = 1,
	TranslationY = 2,
	TranslationZ = 3,
	RotationX = 4,
	RotationY = 5,
	RotationZ = 6,
	Scale = 7,
	ScaleX = 8,
	ScaleY = 9,
	ScaleZ = 10,
	EComponentType_MAX = 11
};

// Object: Enum Engine.EAxisOption
enum class EAxisOption : uint8_t
{
	X = 0,
	Y = 1,
	Z = 2,
	X_Neg = 3,
	Y_Neg = 4,
	Z_Neg = 5,
	Custom = 6,
	EAxisOption_MAX = 7
};

// Object: Enum Engine.EAnimInterpolationType
enum class EAnimInterpolationType : uint8_t
{
	Linear = 0,
	Step = 1,
	EAnimInterpolationType_MAX = 2
};

// Object: Enum Engine.ECurveBlendOption
enum class ECurveBlendOption : uint8_t
{
	Override = 0,
	DoNotOverride = 1,
	NormalizeByWeight = 2,
	BlendByWeight = 3,
	UseBasePose = 4,
	UseMaxValue = 5,
	UseMinValue = 6,
	ECurveBlendOption_MAX = 7
};

// Object: Enum Engine.EAdditiveAnimationType
enum class EAdditiveAnimationType : uint8_t
{
	AAT_None = 0,
	AAT_LocalSpaceBase = 1,
	AAT_RotationOffsetMeshSpace = 2,
	AAT_MAX = 3
};

// Object: Enum Engine.ENotifyFilterType
enum class ENotifyFilterType : uint8_t
{
	NoFiltering = 0,
	LOD = 1,
	ENotifyFilterType_MAX = 2
};

// Object: Enum Engine.EMontageNotifyTickType
enum class EMontageNotifyTickType : uint8_t
{
	Queued = 0,
	BranchingPoint = 1,
	EMontageNotifyTickType_MAX = 2
};

// Object: Enum Engine.EBoneRotationSource
enum class EBoneRotationSource : uint8_t
{
	BRS_KeepComponentSpaceRotation = 0,
	BRS_KeepLocalSpaceRotation = 1,
	BRS_CopyFromTarget = 2,
	BRS_MAX = 3
};

// Object: Enum Engine.EBoneControlSpace
enum class EBoneControlSpace : uint8_t
{
	BCS_WorldSpace = 0,
	BCS_ComponentSpace = 1,
	BCS_ParentBoneSpace = 2,
	BCS_BoneSpace = 3,
	BCS_MAX = 4
};

// Object: Enum Engine.EBoneAxis
enum class EBoneAxis : uint8_t
{
	BA_X = 0,
	BA_Y = 1,
	BA_Z = 2,
	BA_MAX = 3
};

// Object: Enum Engine.EPrimaryAssetCookRule
enum class EPrimaryAssetCookRule : uint8_t
{
	Unknown = 0,
	NeverCook = 1,
	DevelopmentCook = 2,
	DevelopmentAlwaysCook = 3,
	AlwaysCook = 4,
	EPrimaryAssetCookRule_MAX = 5
};

// Object: Enum Engine.ENaturalSoundFalloffMode
enum class ENaturalSoundFalloffMode : uint8_t
{
	Continues = 0,
	Silent = 1,
	Hold = 2,
	ENaturalSoundFalloffMode_MAX = 3
};

// Object: Enum Engine.EAttenuationShape
enum class EAttenuationShape : uint8_t
{
	Sphere = 0,
	Capsule = 1,
	Box = 2,
	Cone = 3,
	EAttenuationShape_MAX = 4
};

// Object: Enum Engine.EAttenuationDistanceModel
enum class EAttenuationDistanceModel : uint8_t
{
	Linear = 0,
	Logarithmic = 1,
	Inverse = 2,
	LogReverse = 3,
	NaturalSound = 4,
	Custom = 5,
	EAttenuationDistanceModel_MAX = 6
};

// Object: Enum Engine.EAudioBusChannels
enum class EAudioBusChannels : uint8_t
{
	Mono = 0,
	Stereo = 1,
	EAudioBusChannels_MAX = 2
};

// Object: Enum Engine.EAudioFaderCurve
enum class EAudioFaderCurve : uint8_t
{
	Linear = 0,
	Logarithmic = 1,
	SCurve = 2,
	Sin = 3,
	Count = 4,
	EAudioFaderCurve_MAX = 5
};

// Object: Enum Engine.EAudioOutputTarget
enum class EAudioOutputTarget : uint8_t
{
	Speaker = 0,
	Controller = 1,
	ControllerFallbackToSpeaker = 2,
	EAudioOutputTarget_MAX = 3
};

// Object: Enum Engine.EMonoChannelUpmixMethod
enum class EMonoChannelUpmixMethod : uint8_t
{
	Linear = 0,
	EqualPower = 1,
	FullVolume = 2,
	EMonoChannelUpmixMethod_MAX = 3
};

// Object: Enum Engine.EPanningMethod
enum class EPanningMethod : uint8_t
{
	Linear = 0,
	EqualPower = 1,
	EPanningMethod_MAX = 2
};

// Object: Enum Engine.EVoiceSampleRate
enum class EVoiceSampleRate : uint16_t
{
	Low16000Hz = 16000,
	Normal24000Hz = 24000,
	EVoiceSampleRate_MAX = 24001
};

// Object: Enum Engine.EAudioVolumeLocationState
enum class EAudioVolumeLocationState : uint8_t
{
	InsideTheVolume = 0,
	OutsideTheVolume = 1,
	EAudioVolumeLocationState_MAX = 2
};

// Object: Enum Engine.EBlendableLocation
enum class EBlendableLocation : uint8_t
{
	BL_AfterTonemapping = 0,
	BL_BeforeTonemapping = 1,
	BL_BeforeTranslucency = 2,
	BL_ReplacingTonemapper = 3,
	BL_SSRInput = 4,
	BL_AfterPostProcessAA = 5,
	BL_MAX = 6
};

// Object: Enum Engine.ENotifyTriggerMode
enum class ENotifyTriggerMode : uint8_t
{
	AllAnimations = 0,
	HighestWeightedAnimation = 1,
	None = 2,
	ENotifyTriggerMode_MAX = 3
};

// Object: Enum Engine.EBlendSpaceAxis
enum class EBlendSpaceAxis : uint8_t
{
	BSA_None = 0,
	BSA_X = 1,
	BSA_Y = 2,
	BSA_Max = 3
};

// Object: Enum Engine.EBlueprintNativizationFlag
enum class EBlueprintNativizationFlag : uint8_t
{
	Disabled = 0,
	Dependency = 1,
	ExplicitlyEnabled = 2,
	EBlueprintNativizationFlag_MAX = 3
};

// Object: Enum Engine.EBlueprintCompileMode
enum class EBlueprintCompileMode : uint8_t
{
	Default = 0,
	Development = 1,
	FinalRelease = 2,
	EBlueprintCompileMode_MAX = 3
};

// Object: Enum Engine.EBlueprintType
enum class EBlueprintType : uint8_t
{
	BPTYPE_Normal = 0,
	BPTYPE_Const = 1,
	BPTYPE_MacroLibrary = 2,
	BPTYPE_Interface = 3,
	BPTYPE_LevelScript = 4,
	BPTYPE_FunctionLibrary = 5,
	BPTYPE_MAX = 6
};

// Object: Enum Engine.EBlueprintStatus
enum class EBlueprintStatus : uint8_t
{
	BS_Unknown = 0,
	BS_Dirty = 1,
	BS_Error = 2,
	BS_UpToDate = 3,
	BS_BeingCreated = 4,
	BS_UpToDateWithWarnings = 5,
	BS_MAX = 6
};

// Object: Enum Engine.EBlueprintModuleLoadResult
enum class EBlueprintModuleLoadResult : uint8_t
{
	Success = 0,
	FileNotFound = 1,
	FileIncompatible = 2,
	CouldNotBeLoadedByOS = 3,
	FailedToInitialize = 4,
	EBlueprintModuleLoadResult_MAX = 5
};

// Object: Enum Engine.EDOFMode
enum class EDOFMode : uint8_t
{
	Default = 0,
	SixDOF = 1,
	YZPlane = 2,
	XZPlane = 3,
	XYPlane = 4,
	CustomPlane = 5,
	None = 6,
	EDOFMode_MAX = 7
};

// Object: Enum Engine.EBrushType
enum class EBrushType : uint8_t
{
	Brush_Default = 0,
	Brush_Add = 1,
	Brush_Subtract = 2,
	Brush_MAX = 3
};

// Object: Enum Engine.ECsgOper
enum class ECsgOper : uint8_t
{
	CSG_Active = 0,
	CSG_Add = 1,
	CSG_Subtract = 2,
	CSG_Intersect = 3,
	CSG_Deintersect = 4,
	CSG_None = 5,
	CSG_MAX = 6
};

// Object: Enum Engine.ECameraShakeDurationType
enum class ECameraShakeDurationType : uint8_t
{
	Fixed = 0,
	Infinite = 1,
	Custom = 2,
	ECameraShakeDurationType_MAX = 3
};

// Object: Enum Engine.ECameraShakeUpdateResultFlags
enum class ECameraShakeUpdateResultFlags : uint8_t
{
	ApplyAsAbsolute = 0,
	SkipAutoScale = 1,
	SkipAutoPlaySpace = 2,
	Default = 3,
	ECameraShakeUpdateResultFlags_MAX = 4
};

// Object: Enum Engine.ECameraShakeAttenuation
enum class ECameraShakeAttenuation : uint8_t
{
	Linear = 0,
	Quadratic = 1,
	ECameraShakeAttenuation_MAX = 2
};

// Object: Enum Engine.ECameraAlphaBlendMode
enum class ECameraAlphaBlendMode : uint8_t
{
	CABM_Linear = 0,
	CABM_Cubic = 1,
	CABM_MAX = 2
};

// Object: Enum Engine.ECameraProjectionMode
enum class ECameraProjectionMode : uint8_t
{
	Perspective = 0,
	Orthographic = 1,
	ECameraProjectionMode_MAX = 2
};

// Object: Enum Engine.ECloudStorageDelegate
enum class ECloudStorageDelegate : uint8_t
{
	CSD_KeyValueReadComplete = 0,
	CSD_KeyValueWriteComplete = 1,
	CSD_ValueChanged = 2,
	CSD_DocumentQueryComplete = 3,
	CSD_DocumentReadComplete = 4,
	CSD_DocumentWriteComplete = 5,
	CSD_DocumentConflictDetected = 6,
	CSD_MAX = 7
};

// Object: Enum Engine.EAngularDriveMode
enum class EAngularDriveMode : uint8_t
{
	SLERP = 0,
	TwistAndSwing = 1,
	EAngularDriveMode_MAX = 2
};

// Object: Enum Engine.PurgeGarbageTimeLimitType
enum class EPurgeGarbageTimeLimitType : uint8_t
{
	tltLow = 0,
	tltMiddle = 1,
	tltHigh = 2,
	tltSuper = 3,
	tltMax = 4,
	PurgeGarbageTimeLimitType_MAX = 5
};

// Object: Enum Engine.ECurveTableMode
enum class ECurveTableMode : uint8_t
{
	Empty = 0,
	SimpleCurves = 1,
	RichCurves = 2,
	ECurveTableMode_MAX = 3
};

// Object: Enum Engine.ECustomAttributeBlendType
enum class ECustomAttributeBlendType : uint8_t
{
	Override = 0,
	Blend = 1,
	ECustomAttributeBlendType_MAX = 2
};

// Object: Enum Engine.EEvaluateCurveTableResult
enum class EEvaluateCurveTableResult : uint8_t
{
	RowFound = 0,
	RowNotFound = 1,
	EEvaluateCurveTableResult_MAX = 2
};

// Object: Enum Engine.PrimitiveGroup
enum class EPrimitiveGroup : uint8_t
{
	PRIMITIVEGROUP_Low = 0,
	PRIMITIVEGROUP_Middle = 1,
	PRIMITIVEGROUP_High = 2,
	PRIMITIVEGROUP_MAX = 3
};

// Object: Enum Engine.EGrammaticalNumber
enum class EGrammaticalNumber : uint8_t
{
	Singular = 0,
	Plural = 1,
	EGrammaticalNumber_MAX = 2
};

// Object: Enum Engine.EGrammaticalGender
enum class EGrammaticalGender : uint8_t
{
	Neuter = 0,
	Masculine = 1,
	Feminine = 2,
	Mixed = 3,
	EGrammaticalGender_MAX = 4
};

// Object: Enum Engine.ECSMCascadeSplitMethod
enum class ECSMCascadeSplitMethod : uint8_t
{
	Standard = 0,
	PracticalSplitScheme = 1,
	ECSMCascadeSplitMethod_MAX = 2
};

// Object: Enum Engine.DistributionParamMode
enum class EDistributionParamMode : uint8_t
{
	DPM_Normal = 0,
	DPM_Abs = 1,
	DPM_Direct = 2,
	DPM_MAX = 3
};

// Object: Enum Engine.EDistributionVectorMirrorFlags
enum class EDistributionVectorMirrorFlags : uint8_t
{
	EDVMF_Same = 0,
	EDVMF_Different = 1,
	EDVMF_Mirror = 2,
	EDVMF_MAX = 3
};

// Object: Enum Engine.EDistributionVectorLockFlags
enum class EDistributionVectorLockFlags : uint8_t
{
	EDVLF_None = 0,
	EDVLF_XY = 1,
	EDVLF_XZ = 2,
	EDVLF_YZ = 3,
	EDVLF_XYZ = 4,
	EDVLF_MAX = 5
};

// Object: Enum Engine.ENodeEnabledState
enum class ENodeEnabledState : uint8_t
{
	Enabled = 0,
	Disabled = 1,
	DevelopmentOnly = 2,
	ENodeEnabledState_MAX = 3
};

// Object: Enum Engine.ENodeAdvancedPins
enum class ENodeAdvancedPins : uint8_t
{
	NoPins = 0,
	Shown = 1,
	Hidden = 2,
	ENodeAdvancedPins_MAX = 3
};

// Object: Enum Engine.ENodeTitleType
enum class ENodeTitleType : uint8_t
{
	FullTitle = 0,
	ListView = 1,
	EditableTitle = 2,
	MenuTitle = 3,
	MAX_TitleTypes = 4,
	ENodeTitleType_MAX = 5
};

// Object: Enum Engine.EPinContainerType
enum class EPinContainerType : uint8_t
{
	None = 0,
	Array = 1,
	Set = 2,
	Map = 3,
	EPinContainerType_MAX = 4
};

// Object: Enum Engine.EEdGraphPinDirection
enum class EEdGraphPinDirection : uint8_t
{
	EGPD_Input = 0,
	EGPD_Output = 1,
	EGPD_MAX = 2
};

// Object: Enum Engine.EBlueprintPinStyleType
enum class EBlueprintPinStyleType : uint8_t
{
	BPST_Original = 0,
	BPST_VariantA = 1,
	BPST_MAX = 2
};

// Object: Enum Engine.ECanCreateConnectionResponse
enum class ECanCreateConnectionResponse : uint8_t
{
	CONNECT_RESPONSE_MAKE = 0,
	CONNECT_RESPONSE_DISALLOW = 1,
	CONNECT_RESPONSE_BREAK_OTHERS_A = 2,
	CONNECT_RESPONSE_BREAK_OTHERS_B = 3,
	CONNECT_RESPONSE_BREAK_OTHERS_AB = 4,
	CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE = 5,
	CONNECT_RESPONSE_MAX = 6
};

// Object: Enum Engine.EGraphType
enum class EGraphType : uint8_t
{
	GT_Function = 0,
	GT_Ubergraph = 1,
	GT_Macro = 2,
	GT_Animation = 3,
	GT_StateMachine = 4,
	GT_MAX = 5
};

// Object: Enum Engine.ETransitionType
enum class ETransitionType : uint8_t
{
	None = 0,
	Paused = 1,
	Loading = 2,
	Saving = 3,
	Connecting = 4,
	Precaching = 5,
	WaitingToConnect = 6,
	MAX = 7
};

// Object: Enum Engine.EFullyLoadPackageType
enum class EFullyLoadPackageType : uint8_t
{
	FULLYLOAD_Map = 0,
	FULLYLOAD_Game_PreLoadClass = 1,
	FULLYLOAD_Game_PostLoadClass = 2,
	FULLYLOAD_Always = 3,
	FULLYLOAD_Mutator = 4,
	FULLYLOAD_MAX = 5
};

// Object: Enum Engine.EViewModeIndex
enum class EViewModeIndex : uint8_t
{
	VMI_BrushWireframe = 0,
	VMI_Wireframe = 1,
	VMI_Unlit = 2,
	VMI_Lit = 3,
	VMI_Lit_DetailLighting = 4,
	VMI_LightingOnly = 5,
	VMI_LightComplexity = 6,
	VMI_ShaderComplexity = 8,
	VMI_LightmapDensity = 9,
	VMI_LitLightmapDensity = 10,
	VMI_ReflectionOverride = 11,
	VMI_VisualizeBuffer = 12,
	VMI_StationaryLightOverlap = 14,
	VMI_CollisionPawn = 15,
	VMI_CollisionVisibility = 16,
	VMI_LODColoration = 18,
	VMI_QuadOverdraw = 19,
	VMI_PrimitiveDistanceAccuracy = 20,
	VMI_MeshUVDensityAccuracy = 21,
	VMI_ShaderComplexityWithQuadOverdraw = 22,
	VMI_HLODColoration = 23,
	VMI_GroupLODColoration = 24,
	VMI_MaterialTextureScaleAccuracy = 25,
	VMI_RequiredTextureResolution = 26,
	VMI_PathTracing = 27,
	VMI_RayTracingDebug = 28,
	VMI_Overdraw = 29,
	VMI_SpSlopeWorld = 30,
	VMI_SpPassabilityCheck = 31,
	VMI_LightCount = 32,
	VMI_Max = 33,
	VMI_Unknown = 255
};

// Object: Enum Engine.EDemoPlayFailure
enum class EDemoPlayFailure : uint8_t
{
	Generic = 0,
	DemoNotFound = 1,
	Corrupt = 2,
	InvalidVersion = 3,
	InitBase = 4,
	GameSpecificHeader = 5,
	ReplayStreamerInternal = 6,
	LoadMap = 7,
	Serialization = 8,
	EDemoPlayFailure_MAX = 9
};

// Object: Enum Engine.ENetworkLagState
enum class ENetworkLagState : uint8_t
{
	NotLagging = 0,
	Lagging = 1,
	ENetworkLagState_MAX = 2
};

// Object: Enum Engine.EMouseCaptureMode
enum class EMouseCaptureMode : uint8_t
{
	NoCapture = 0,
	CapturePermanently = 1,
	CapturePermanently_IncludingInitialMouseDown = 2,
	CaptureDuringMouseDown = 3,
	CaptureDuringRightMouseDown = 4,
	EMouseCaptureMode_MAX = 5
};

// Object: Enum Engine.ECustomTimeStepSynchronizationState
enum class ECustomTimeStepSynchronizationState : uint8_t
{
	Closed = 0,
	Error = 1,
	Synchronized = 2,
	Synchronizing = 3,
	ECustomTimeStepSynchronizationState_MAX = 4
};

// Object: Enum Engine.EMeshBufferAccess
enum class EMeshBufferAccess : uint8_t
{
	Default = 0,
	ForceCPUAndGPU = 1,
	EMeshBufferAccess_MAX = 2
};

// Object: Enum Engine.EComponentSocketType
enum class EComponentSocketType : uint8_t
{
	Invalid = 0,
	Bone = 1,
	Socket = 2,
	EComponentSocketType_MAX = 3
};

// Object: Enum Engine.EPhysicalMaterialMaskColor
enum class EPhysicalMaterialMaskColor : uint8_t
{
	Red = 0,
	Green = 1,
	Blue = 2,
	Cyan = 3,
	Magenta = 4,
	Yellow = 5,
	White = 6,
	Black = 7,
	MAX = 8
};

// Object: Enum Engine.EWalkableSlopeBehavior
enum class EWalkableSlopeBehavior : uint8_t
{
	WalkableSlope_Default = 0,
	WalkableSlope_Increase = 1,
	WalkableSlope_Decrease = 2,
	WalkableSlope_Unwalkable = 3,
	WalkableSlope_Max = 4
};

// Object: Enum Engine.ESplitRGBA
enum class ESplitRGBA : uint8_t
{
	R = 0,
	G = 1,
	B = 2,
	A = 3,
	None = 4,
	ESplitRGBA_MAX = 5
};

// Object: Enum Engine.FAnimSkipFrameExMode
enum class EFAnimSkipFrameExMode : uint8_t
{
	NotEnabled = 0,
	AlwaysTickAndInterpolate = 1,
	FAnimSkipFrameExMode_MAX = 2
};

// Object: Enum Engine.EUpdateRateShiftBucket
enum class EUpdateRateShiftBucket : uint8_t
{
	ShiftBucket0 = 0,
	ShiftBucket1 = 1,
	ShiftBucket2 = 2,
	ShiftBucket3 = 3,
	ShiftBucket4 = 4,
	ShiftBucket5 = 5,
	ShiftBucketMax = 6,
	EUpdateRateShiftBucket_MAX = 7
};

// Object: Enum Engine.EShadowMapFlags
enum class EShadowMapFlags : uint8_t
{
	SMF_None = 0,
	SMF_Streamed = 1,
	SMF_MAX = 2
};

// Object: Enum Engine.ELightMapPaddingType
enum class ELightMapPaddingType : uint8_t
{
	LMPT_NormalPadding = 0,
	LMPT_PrePadding = 1,
	LMPT_NoPadding = 2,
	LMPT_MAX = 3
};

// Object: Enum Engine.ECollisionEnabled
enum class ECollisionEnabled : uint8_t
{
	NoCollision = 0,
	QueryOnly = 1,
	PhysicsOnly = 2,
	QueryAndPhysics = 3,
	ECollisionEnabled_MAX = 4
};

// Object: Enum Engine.ETimelineSigType
enum class ETimelineSigType : uint8_t
{
	ETS_EventSignature = 0,
	ETS_FloatSignature = 1,
	ETS_VectorSignature = 2,
	ETS_LinearColorSignature = 3,
	ETS_InvalidSignature = 4,
	ETS_MAX = 5
};

// Object: Enum Engine.EFilterInterpolationType
enum class EFilterInterpolationType : uint8_t
{
	BSIT_Average = 0,
	BSIT_Linear = 1,
	BSIT_Cubic = 2,
	BSIT_MAX = 3
};

// Object: Enum Engine.ECollisionResponse
enum class ECollisionResponse : uint8_t
{
	ECR_Ignore = 0,
	ECR_Overlap = 1,
	ECR_Block = 2,
	ECR_MAX = 3
};

// Object: Enum Engine.EOverlapFilterOption
enum class EOverlapFilterOption : uint8_t
{
	OverlapFilter_All = 0,
	OverlapFilter_DynamicOnly = 1,
	OverlapFilter_StaticOnly = 2,
	OverlapFilter_MAX = 3
};

// Object: Enum Engine.ELightingBuildQuality
enum class ELightingBuildQuality : uint8_t
{
	Quality_Preview = 0,
	Quality_Medium = 1,
	Quality_High = 2,
	Quality_Production = 3,
	Quality_MAX = 4
};

// Object: Enum Engine.EMaterialShadingRate
enum class EMaterialShadingRate : uint8_t
{
	MSR_1x1 = 0,
	MSR_2x1 = 1,
	MSR_1x2 = 2,
	MSR_2x2 = 3,
	MSR_4x2 = 4,
	MSR_2x4 = 5,
	MSR_4x4 = 6,
	MSR_Count = 7,
	MSR_MAX = 8
};

// Object: Enum Engine.EMaterialStencilCompare
enum class EMaterialStencilCompare : uint8_t
{
	MSC_Less = 0,
	MSC_LessEqual = 1,
	MSC_Greater = 2,
	MSC_GreaterEqual = 3,
	MSC_Equal = 4,
	MSC_NotEqual = 5,
	MSC_Never = 6,
	MSC_Always = 7,
	MSC_Count = 8,
	MSC_MAX = 9
};

// Object: Enum Engine.EMaterialSamplerType
enum class EMaterialSamplerType : uint8_t
{
	SAMPLERTYPE_Color = 0,
	SAMPLERTYPE_Grayscale = 1,
	SAMPLERTYPE_Alpha = 2,
	SAMPLERTYPE_Normal = 3,
	SAMPLERTYPE_Masks = 4,
	SAMPLERTYPE_DistanceFieldFont = 5,
	SAMPLERTYPE_LinearColor = 6,
	SAMPLERTYPE_LinearGrayscale = 7,
	SAMPLERTYPE_Data = 8,
	SAMPLERTYPE_External = 9,
	SAMPLERTYPE_VirtualColor = 10,
	SAMPLERTYPE_VirtualGrayscale = 11,
	SAMPLERTYPE_VirtualAlpha = 12,
	SAMPLERTYPE_VirtualNormal = 13,
	SAMPLERTYPE_VirtualMasks = 14,
	SAMPLERTYPE_VirtualLinearColor = 15,
	SAMPLERTYPE_VirtualLinearGrayscale = 16,
	SAMPLERTYPE_MAX = 17
};

// Object: Enum Engine.EMaterialTessellationMode
enum class EMaterialTessellationMode : uint8_t
{
	MTM_NoTessellation = 0,
	MTM_FlatTessellation = 1,
	MTM_PNTriangles = 2,
	MTM_MAX = 3
};

// Object: Enum Engine.EMaterialShadingModel
enum class EMaterialShadingModel : uint8_t
{
	MSM_Unlit = 0,
	MSM_DefaultLit = 1,
	MSM_Subsurface = 2,
	MSM_PreintegratedSkin = 3,
	MSM_ClearCoat = 4,
	MSM_SubsurfaceProfile = 5,
	MSM_TwoSidedFoliage = 6,
	MSM_Hair = 7,
	MSM_Cloth = 8,
	MSM_Eye = 9,
	MSM_SingleLayerWater = 10,
	MSM_ThinTranslucent = 11,
	MSM_SGSSS = 12,
	MSM_DKHair = 13,
	MSM_Iridescence = 14,
	MSM_HOK_SSS = 15,
	MSM_CustomBRDFPreIntegratedSkin = 16,
	MSM_MoibleKajiyaKayHair = 17,
	MSM_ShortFur = 18,
	MSM_YM_Character = 19,
	MSM_SimpleLit = 20,
	MSM_CustomLit = 21,
	MSM_NUM = 22,
	MSM_FromMaterialExpression = 23,
	MSM_MAX = 24
};

// Object: Enum Engine.EParticleCollisionMode
enum class EParticleCollisionMode : uint8_t
{
	SceneDepth = 0,
	DistanceField = 1,
	EParticleCollisionMode_MAX = 2
};

// Object: Enum Engine.ETrailWidthMode
enum class ETrailWidthMode : uint8_t
{
	ETrailWidthMode_FromCentre = 0,
	ETrailWidthMode_FromFirst = 1,
	ETrailWidthMode_FromSecond = 2,
	ETrailWidthMode_MAX = 3
};

// Object: Enum Engine.EGBufferFormat
enum class EGBufferFormat : uint8_t
{
	Force8BitsPerChannel = 0,
	Default = 1,
	HighPrecisionNormals = 3,
	Force16BitsPerChannel = 5,
	EGBufferFormat_MAX = 6
};

// Object: Enum Engine.ESceneCaptureCompositeMode
enum class ESceneCaptureCompositeMode : uint8_t
{
	SCCM_Overwrite = 0,
	SCCM_Additive = 1,
	SCCM_Composite = 2,
	SCCM_MAX = 3
};

// Object: Enum Engine.ESceneCaptureSource
enum class ESceneCaptureSource : uint8_t
{
	SCS_SceneColorHDR = 0,
	SCS_SceneColorHDRNoAlpha = 1,
	SCS_FinalColorLDR = 2,
	SCS_SceneColorSceneDepth = 3,
	SCS_SceneDepth = 4,
	SCS_DeviceDepth = 5,
	SCS_Normal = 6,
	SCS_BaseColor = 7,
	SCS_FinalColorHDR = 8,
	SCS_FinalToneCurveHDR = 9,
	SCS_ImpostorTex = 10,
	SCS_MAX = 11
};

// Object: Enum Engine.ETranslucentSortPolicy
enum class ETranslucentSortPolicy : uint8_t
{
	SortByDistance = 0,
	SortByProjectedZ = 1,
	SortAlongAxis = 2,
	ETranslucentSortPolicy_MAX = 3
};

// Object: Enum Engine.ERefractionMode
enum class ERefractionMode : uint8_t
{
	RM_IndexOfRefraction = 0,
	RM_PixelNormalOffset = 1,
	RM_MAX = 2
};

// Object: Enum Engine.ETranslucencyLightingMode
enum class ETranslucencyLightingMode : uint8_t
{
	TLM_VolumetricNonDirectional = 0,
	TLM_VolumetricDirectional = 1,
	TLM_VolumetricPerVertexNonDirectional = 2,
	TLM_VolumetricPerVertexDirectional = 3,
	TLM_Surface = 4,
	TLM_SurfacePerPixelLighting = 5,
	TLM_MAX = 6
};

// Object: Enum Engine.ESamplerSourceMode
enum class ESamplerSourceMode : uint8_t
{
	SSM_FromTextureAsset = 0,
	SSM_Wrap_WorldGroupSettings = 1,
	SSM_Clamp_WorldGroupSettings = 2,
	SSM_MAX = 3
};

// Object: Enum Engine.EBlendMode
enum class EBlendMode : uint8_t
{
	BLEND_Opaque = 0,
	BLEND_Masked = 1,
	BLEND_Translucent = 2,
	BLEND_Additive = 3,
	BLEND_Modulate = 4,
	BLEND_AlphaComposite = 5,
	BLEND_AlphaHoldout = 6,
	BLEND_MaxSrcDst = 7,
	BLEND_ManuallyBlend = 8,
	BLEND_MAX = 9
};

// Object: Enum Engine.EOcclusionCombineMode
enum class EOcclusionCombineMode : uint8_t
{
	OCM_Minimum = 0,
	OCM_Multiply = 1,
	OCM_MAX = 2
};

// Object: Enum Engine.ELightmapType
enum class ELightmapType : uint8_t
{
	Default = 0,
	ForceSurface = 1,
	ForceVolumetric = 2,
	ELightmapType_MAX = 3
};

// Object: Enum Engine.EOutlineRenderMode
enum class EOutlineRenderMode : uint8_t
{
	AlwaysVisible = 0,
	AlwaysOccluded = 1,
	OcclusionControlled = 2,
	OcclusionCorrected = 3,
	EOutlineRenderMode_MAX = 4
};

// Object: Enum Engine.ESceneDepthPriorityGroup
enum class ESceneDepthPriorityGroup : uint8_t
{
	SDPG_World = 0,
	SDPG_Foreground = 1,
	SDPG_MAX = 2
};

// Object: Enum Engine.EAspectRatioAxisConstraint
enum class EAspectRatioAxisConstraint : uint8_t
{
	AspectRatio_MaintainYFOV = 0,
	AspectRatio_MaintainXFOV = 1,
	AspectRatio_MajorAxisFOV = 2,
	AspectRatio_MAX = 3
};

// Object: Enum Engine.EFontCacheType
enum class EFontCacheType : uint8_t
{
	Offline = 0,
	Runtime = 1,
	EFontCacheType_MAX = 2
};

// Object: Enum Engine.EFontImportCharacterSet
enum class EFontImportCharacterSet : uint8_t
{
	FontICS_Default = 0,
	FontICS_Ansi = 1,
	FontICS_Symbol = 2,
	FontICS_MAX = 3
};

// Object: Enum Engine.EStandbyType
enum class EStandbyType : uint8_t
{
	STDBY_Rx = 0,
	STDBY_Tx = 1,
	STDBY_BadPing = 2,
	STDBY_MAX = 3
};

// Object: Enum Engine.ESuggestProjVelocityTraceOption
enum class ESuggestProjVelocityTraceOption : uint8_t
{
	DoNotTrace = 0,
	TraceFullPath = 1,
	OnlyTraceWhileAscending = 2,
	ESuggestProjVelocityTraceOption_MAX = 3
};

// Object: Enum Engine.EWindowMode
enum class EWindowMode : uint8_t
{
	Fullscreen = 0,
	WindowedFullscreen = 1,
	Windowed = 2,
	EWindowMode_MAX = 3
};

// Object: Enum Engine.EHitProxyPriority
enum class EHitProxyPriority : uint8_t
{
	HPP_World = 0,
	HPP_Wireframe = 1,
	HPP_Foreground = 2,
	HPP_UI = 3,
	HPP_MAX = 4
};

// Object: Enum Engine.EMoeUGCGameType
enum class EMoeUGCGameType : uint8_t
{
	eMoeUGCUnknown = 0,
	eMoeUGCEditor = 1,
	eMoeUGCTryPlay = 2,
	eMoeUGCPlaying = 3,
	eMoeUGC = 4,
	EMoeUGCGameType_MAX = 5
};

// Object: Enum Engine.EImportanceWeight
enum class EImportanceWeight : uint8_t
{
	Luminance = 0,
	Red = 1,
	Green = 2,
	Blue = 3,
	Alpha = 4,
	EImportanceWeight_MAX = 5
};

// Object: Enum Engine.EAdManagerDelegate
enum class EAdManagerDelegate : uint8_t
{
	AMD_ClickedBanner = 0,
	AMD_UserClosedAd = 1,
	AMD_MAX = 2
};

// Object: Enum Engine.EAnimAlphaInputType
enum class EAnimAlphaInputType : uint8_t
{
	Float = 0,
	Bool = 1,
	Curve = 2,
	FloatUnClamp = 3,
	EAnimAlphaInputType_MAX = 4
};

// Object: Enum Engine.ESplatPointType
enum class ESplatPointType : uint8_t
{
	BaseActor = 0,
	BaseBone = 1,
	ESplatPointType_MAX = 2
};

// Object: Enum Engine.EInteractiveWaterDrivenType
enum class EInteractiveWaterDrivenType : uint8_t
{
	VelocityPure = 0,
	VelocityFramerate = 1,
	EInteractiveWaterDrivenType_MAX = 2
};

// Object: Enum Engine.ETrackActiveCondition
enum class ETrackActiveCondition : uint8_t
{
	ETAC_Always = 0,
	ETAC_GoreEnabled = 1,
	ETAC_GoreDisabled = 2,
	ETAC_MAX = 3
};

// Object: Enum Engine.EInterpTrackMoveRotMode
enum class EInterpTrackMoveRotMode : uint8_t
{
	IMR_Keyframed = 0,
	IMR_LookAtGroup = 1,
	IMR_Ignore = 2,
	IMR_MAX = 3
};

// Object: Enum Engine.EInterpMoveAxis
enum class EInterpMoveAxis : uint8_t
{
	AXIS_TranslationX = 0,
	AXIS_TranslationY = 1,
	AXIS_TranslationZ = 2,
	AXIS_RotationX = 3,
	AXIS_RotationY = 4,
	AXIS_RotationZ = 5,
	AXIS_MAX = 6
};

// Object: Enum Engine.ETrackToggleAction
enum class ETrackToggleAction : uint8_t
{
	ETTA_Off = 0,
	ETTA_On = 1,
	ETTA_Toggle = 2,
	ETTA_Trigger = 3,
	ETTA_MAX = 4
};

// Object: Enum Engine.EVisibilityTrackCondition
enum class EVisibilityTrackCondition : uint8_t
{
	EVTC_Always = 0,
	EVTC_GoreEnabled = 1,
	EVTC_GoreDisabled = 2,
	EVTC_MAX = 3
};

// Object: Enum Engine.EVisibilityTrackAction
enum class EVisibilityTrackAction : uint8_t
{
	EVTA_Hide = 0,
	EVTA_Show = 1,
	EVTA_Toggle = 2,
	EVTA_MAX = 3
};

// Object: Enum Engine.ESlateGesture
enum class ESlateGesture : uint8_t
{
	None = 0,
	Scroll = 1,
	Magnify = 2,
	Swipe = 3,
	Rotate = 4,
	LongPress = 5,
	ESlateGesture_MAX = 6
};

// Object: Enum Engine.EMIDCreationFlags
enum class EMIDCreationFlags : uint8_t
{
	None = 0,
	Transient = 1,
	EMIDCreationFlags_MAX = 2
};

// Object: Enum Engine.EMatrixColumns
enum class EMatrixColumns : uint8_t
{
	First = 0,
	Second = 1,
	Third = 2,
	Fourth = 3,
	EMatrixColumns_MAX = 4
};

// Object: Enum Engine.ELerpInterpolationMode
enum class ELerpInterpolationMode : uint8_t
{
	QuatInterp = 0,
	EulerInterp = 1,
	DualQuatInterp = 2,
	ELerpInterpolationMode_MAX = 3
};

// Object: Enum Engine.EEasingFunc
enum class EEasingFunc : uint8_t
{
	Linear = 0,
	Step = 1,
	SinusoidalIn = 2,
	SinusoidalOut = 3,
	SinusoidalInOut = 4,
	EaseIn = 5,
	EaseOut = 6,
	EaseInOut = 7,
	ExpoIn = 8,
	ExpoOut = 9,
	ExpoInOut = 10,
	CircularIn = 11,
	CircularOut = 12,
	CircularInOut = 13,
	EEasingFunc_MAX = 14
};

// Object: Enum Engine.ERoundingMode
enum class ERoundingMode : uint8_t
{
	HalfToEven = 0,
	HalfFromZero = 1,
	HalfToZero = 2,
	FromZero = 3,
	ToZero = 4,
	ToNegativeInfinity = 5,
	ToPositiveInfinity = 6,
	ERoundingMode_MAX = 7
};

// Object: Enum Engine.EStreamingVolumeUsage
enum class EStreamingVolumeUsage : uint8_t
{
	SVB_Loading = 0,
	SVB_LoadingAndVisibility = 1,
	SVB_VisibilityBlockingOnLoad = 2,
	SVB_BlockingOnLoad = 3,
	SVB_LoadingNotVisible = 4,
	SVB_MAX = 5
};

// Object: Enum Engine.ELocalFogMode
enum class ELocalFogMode : uint8_t
{
	LocalHeightFog = 0,
	LocalSphereFog = 1,
	ELocalFogMode_MAX = 2
};

// Object: Enum Engine.ESyncOption
enum class ESyncOption : uint8_t
{
	Drive = 0,
	Passive = 1,
	Disabled = 2,
	ESyncOption_MAX = 3
};

// Object: Enum Engine.EMaterialDecalResponse
enum class EMaterialDecalResponse : uint8_t
{
	MDR_None = 0,
	MDR_ColorNormalRoughness = 1,
	MDR_Color = 2,
	MDR_ColorNormal = 3,
	MDR_ColorRoughness = 4,
	MDR_Normal = 5,
	MDR_NormalRoughness = 6,
	MDR_Roughness = 7,
	MDR_MAX = 8
};

// Object: Enum Engine.EDecalBlendMode
enum class EDecalBlendMode : uint8_t
{
	DBM_Translucent = 0,
	DBM_Stain = 1,
	DBM_Normal = 2,
	DBM_Emissive = 3,
	DBM_DBuffer_ColorNormalRoughness = 4,
	DBM_DBuffer_Color = 5,
	DBM_DBuffer_ColorNormal = 6,
	DBM_DBuffer_ColorRoughness = 7,
	DBM_DBuffer_Normal = 8,
	DBM_DBuffer_NormalRoughness = 9,
	DBM_DBuffer_Roughness = 10,
	DBM_DBuffer_Emissive = 11,
	DBM_DBuffer_AlphaComposite = 12,
	DBM_DBuffer_EmissiveAlphaComposite = 13,
	DBM_Volumetric_DistanceFunction = 14,
	DBM_AlphaComposite = 15,
	DBM_AmbientOcclusion = 16,
	DBM_MAX = 17
};

// Object: Enum Engine.FMaterialParamCalcNodeType
enum class EFMaterialParamCalcNodeType : uint8_t
{
	MPCNT_Constant = 0,
	MPCNT_Constant2Vector = 1,
	MPCNT_Constant3Vector = 2,
	MPCNT_Constant4Vector = 3,
	MPCNT_ScalarParameter = 4,
	MPCNT_VectorParameter = 5,
	MPCNT_Add = 6,
	MPCNT_Subtract = 7,
	MPCNT_Multiply = 8,
	MPCNT_Divide = 9,
	MPCNT_AppendVector = 10,
	MPCNT_ComponentMask = 11,
	MPCNT_Abs = 12,
	MPCNT_Ceil = 13,
	MPCNT_Clamp = 14,
	MPCNT_Cos = 15,
	MPCNT_Cross = 16,
	MPCNT_Dot = 17,
	MPCNT_Floor = 18,
	MPCNT_FMod = 19,
	MPCNT_Frac = 20,
	MPCNT_Max = 21,
	MPCNT_Min = 22,
	MPCNT_Normalize = 23,
	MPCNT_Power = 24,
	MPCNT_Saturate = 25,
	MPCNT_Sign = 26,
	MPCNT_Sin = 27,
	MPCNT_SquareRoot = 28,
	MPCNT_Step = 29,
	MPCNT_SmoothStep = 30,
	MPCNT_Trunc = 31,
	MPCNT_Lerp = 32
};

// Object: Enum Engine.ETextureColorChannel
enum class ETextureColorChannel : uint8_t
{
	TCC_Red = 0,
	TCC_Green = 1,
	TCC_Blue = 2,
	TCC_Alpha = 3,
	TCC_MAX = 4
};

// Object: Enum Engine.EMaterialAttributeBlend
enum class EMaterialAttributeBlend : uint8_t
{
	Blend = 0,
	UseA = 1,
	UseB = 2,
	EMaterialAttributeBlend_MAX = 3
};

// Object: Enum Engine.EMaterialCustomStructMemberType
enum class EMaterialCustomStructMemberType : uint8_t
{
	Float1 = 0,
	Float2 = 1,
	Float3 = 2,
	Float4 = 3,
	EMaterialCustomStructMemberType_MAX = 4
};

// Object: Enum Engine.EMaterialExpressionBranchComparison
enum class EMaterialExpressionBranchComparison : uint8_t
{
	Greater = 0,
	GreaterEqual = 1,
	Less = 2,
	LessEqual = 3,
	Equal = 4,
	AlmostEqual = 5,
	NotEqual = 6,
	AlmostNotEqual = 7,
	EMaterialExpressionBranchComparison_MAX = 8
};

// Object: Enum Engine.EChannelMaskParameterColor
enum class EChannelMaskParameterColor : uint8_t
{
	Red = 0,
	Green = 1,
	Blue = 2,
	Alpha = 3,
	EChannelMaskParameterColor_MAX = 4
};

// Object: Enum Engine.EClampMode
enum class EClampMode : uint8_t
{
	CMODE_Clamp = 0,
	CMODE_ClampMin = 1,
	CMODE_ClampMax = 2,
	CMODE_MAX = 3
};

// Object: Enum Engine.ECustomMaterialOutputType
enum class ECustomMaterialOutputType : uint8_t
{
	CMOT_Float1 = 0,
	CMOT_Float2 = 1,
	CMOT_Float3 = 2,
	CMOT_Float4 = 3,
	CMOT_MaterialAttributes = 4,
	CMOT_MAX = 5
};

// Object: Enum Engine.EDepthOfFieldFunctionValue
enum class EDepthOfFieldFunctionValue : uint8_t
{
	TDOF_NearAndFarMask = 0,
	TDOF_NearMask = 1,
	TDOF_FarMask = 2,
	TDOF_CircleOfConfusionRadius = 3,
	TDOF_MAX = 4
};

// Object: Enum Engine.EFunctionInputType
enum class EFunctionInputType : uint8_t
{
	FunctionInput_Scalar = 0,
	FunctionInput_Vector2 = 1,
	FunctionInput_Vector3 = 2,
	FunctionInput_Vector4 = 3,
	FunctionInput_Texture2D = 4,
	FunctionInput_TextureCube = 5,
	FunctionInput_Texture2DArray = 6,
	FunctionInput_VolumeTexture = 7,
	FunctionInput_StaticBool = 8,
	FunctionInput_MaterialAttributes = 9,
	FunctionInput_TextureExternal = 10,
	FunctionInput_MAX = 11
};

// Object: Enum Engine.ENoiseFunction
enum class ENoiseFunction : uint8_t
{
	NOISEFUNCTION_SimplexTex = 0,
	NOISEFUNCTION_GradientTex = 1,
	NOISEFUNCTION_GradientTex3D = 2,
	NOISEFUNCTION_GradientALU = 3,
	NOISEFUNCTION_ValueALU = 4,
	NOISEFUNCTION_VoronoiALU = 5,
	NOISEFUNCTION_MAX = 6
};

// Object: Enum Engine.ERuntimeVirtualTextureTextureAddressMode
enum class ERuntimeVirtualTextureTextureAddressMode : uint8_t
{
	RVTTA_Clamp = 0,
	RVTTA_Wrap = 1,
	RVTTA_MAX = 2
};

// Object: Enum Engine.ERuntimeVirtualTextureMipValueMode
enum class ERuntimeVirtualTextureMipValueMode : uint8_t
{
	RVTMVM_None = 0,
	RVTMVM_MipLevel = 1,
	RVTMVM_MipBias = 2,
	RVTMVM_MAX = 3
};

// Object: Enum Engine.EMaterialSceneAttributeInputMode
enum class EMaterialSceneAttributeInputMode : uint8_t
{
	Coordinates = 0,
	OffsetFraction = 1,
	EMaterialSceneAttributeInputMode_MAX = 2
};

// Object: Enum Engine.ESpeedTreeLODType
enum class ESpeedTreeLODType : uint8_t
{
	STLOD_Pop = 0,
	STLOD_Smooth = 1,
	STLOD_MAX = 2
};

// Object: Enum Engine.ESpeedTreeWindType
enum class ESpeedTreeWindType : uint8_t
{
	STW_None = 0,
	STW_Fastest = 1,
	STW_Fast = 2,
	STW_Better = 3,
	STW_Best = 4,
	STW_Palm = 5,
	STW_BestPlus = 6,
	STW_MAX = 7
};

// Object: Enum Engine.ESpeedTreeGeometryType
enum class ESpeedTreeGeometryType : uint8_t
{
	STG_Branch = 0,
	STG_Frond = 1,
	STG_Leaf = 2,
	STG_FacingLeaf = 3,
	STG_Billboard = 4,
	STG_MAX = 5
};

// Object: Enum Engine.EMaterialExposedTextureProperty
enum class EMaterialExposedTextureProperty : uint8_t
{
	TMTM_TextureSize = 0,
	TMTM_TexelSize = 1,
	TMTM_MAX = 2
};

// Object: Enum Engine.ETextureMipValueMode
enum class ETextureMipValueMode : uint8_t
{
	TMVM_None = 0,
	TMVM_MipLevel = 1,
	TMVM_MipBias = 2,
	TMVM_Derivative = 3,
	TMVM_MAX = 4
};

// Object: Enum Engine.EMaterialVectorCoordTransform
enum class EMaterialVectorCoordTransform : uint8_t
{
	TRANSFORM_Tangent = 0,
	TRANSFORM_Local = 1,
	TRANSFORM_World = 2,
	TRANSFORM_View = 3,
	TRANSFORM_Camera = 4,
	TRANSFORM_ParticleWorld = 5,
	TRANSFORM_MAX = 6
};

// Object: Enum Engine.EMaterialVectorCoordTransformSource
enum class EMaterialVectorCoordTransformSource : uint8_t
{
	TRANSFORMSOURCE_Tangent = 0,
	TRANSFORMSOURCE_Local = 1,
	TRANSFORMSOURCE_World = 2,
	TRANSFORMSOURCE_View = 3,
	TRANSFORMSOURCE_Camera = 4,
	TRANSFORMSOURCE_ParticleWorld = 5,
	TRANSFORMSOURCE_MAX = 6
};

// Object: Enum Engine.EMaterialPositionTransformSource
enum class EMaterialPositionTransformSource : uint8_t
{
	TRANSFORMPOSSOURCE_Local = 0,
	TRANSFORMPOSSOURCE_World = 1,
	TRANSFORMPOSSOURCE_TranslatedWorld = 2,
	TRANSFORMPOSSOURCE_View = 3,
	TRANSFORMPOSSOURCE_Camera = 4,
	TRANSFORMPOSSOURCE_Particle = 5,
	TRANSFORMPOSSOURCE_Skin = 6,
	TRANSFORMPOSSOURCE_MAX = 7
};

// Object: Enum Engine.EVectorNoiseFunction
enum class EVectorNoiseFunction : uint8_t
{
	VNF_CellnoiseALU = 0,
	VNF_VectorALU = 1,
	VNF_GradientALU = 2,
	VNF_CurlALU = 3,
	VNF_VoronoiALU = 4,
	VNF_MAX = 5
};

// Object: Enum Engine.EMaterialExposedViewProperty
enum class EMaterialExposedViewProperty : uint8_t
{
	MEVP_BufferSize = 0,
	MEVP_FieldOfView = 1,
	MEVP_TanHalfFieldOfView = 2,
	MEVP_ViewSize = 3,
	MEVP_WorldSpaceViewPosition = 4,
	MEVP_WorldSpaceCameraPosition = 5,
	MEVP_ViewportOffset = 6,
	MEVP_TemporalSampleCount = 7,
	MEVP_TemporalSampleIndex = 8,
	MEVP_TemporalSampleOffset = 9,
	MEVP_RuntimeVirtualTextureOutputLevel = 10,
	MEVP_RuntimeVirtualTextureOutputDerivative = 11,
	MEVP_PreExposure = 12,
	MEVP_RuntimeVirtualTextureMaxLevel = 13,
	MEVP_MAX = 14
};

// Object: Enum Engine.EWorldPositionIncludedOffsets
enum class EWorldPositionIncludedOffsets : uint8_t
{
	WPT_Default = 0,
	WPT_ExcludeAllShaderOffsets = 1,
	WPT_CameraRelative = 2,
	WPT_CameraRelativeNoOffsets = 3,
	WPT_MAX = 4
};

// Object: Enum Engine.EMaterialFunctionUsage
enum class EMaterialFunctionUsage : uint8_t
{
	Default = 0,
	MaterialLayer = 1,
	MaterialLayerBlend = 2,
	EMaterialFunctionUsage_MAX = 3
};

// Object: Enum Engine.EMaterialUsage
enum class EMaterialUsage : uint8_t
{
	MATUSAGE_SkeletalMesh = 0,
	MATUSAGE_ParticleSprites = 1,
	MATUSAGE_BeamTrails = 2,
	MATUSAGE_MeshParticles = 3,
	MATUSAGE_ParticleDecals = 4,
	MATUSAGE_StaticLighting = 5,
	MATUSAGE_MorphTargets = 6,
	MATUSAGE_SplineMesh = 7,
	MATUSAGE_InstancedStaticMeshes = 8,
	MATUSAGE_GeometryCollections = 9,
	MATUSAGE_Clothing = 10,
	MATUSAGE_NiagaraSprites = 11,
	MATUSAGE_NiagaraRibbons = 12,
	MATUSAGE_NiagaraMeshParticles = 13,
	MATUSAGE_GeometryCache = 14,
	MATUSAGE_Water = 15,
	MATUSAGE_HairStrands = 16,
	MATUSAGE_LidarPointCloud = 17,
	MATUSAGE_VirtualHeightfieldMesh = 18,
	MATUSAGE_FurShell = 19,
	MATUSAGE_MAX = 20
};

// Object: Enum Engine.EMaterialLayerLinkState
enum class EMaterialLayerLinkState : uint8_t
{
	Uninitialized = 0,
	LinkedToParent = 1,
	UnlinkedFromParent = 2,
	NotFromParent = 3,
	EMaterialLayerLinkState_MAX = 4
};

// Object: Enum Engine.EMaterialParameterAssociation
enum class EMaterialParameterAssociation : uint8_t
{
	LayerParameter = 0,
	BlendParameter = 1,
	GlobalParameter = 2,
	EMaterialParameterAssociation_MAX = 3
};

// Object: Enum Engine.EMaterialMergeType
enum class EMaterialMergeType : uint8_t
{
	MaterialMergeType_Default = 0,
	MaterialMergeType_Simplygon = 1,
	MaterialMergeType_MAX = 2
};

// Object: Enum Engine.ETextureSizingType
enum class ETextureSizingType : uint8_t
{
	TextureSizingType_UseSingleTextureSize = 0,
	TextureSizingType_UseAutomaticBiasedSizes = 1,
	TextureSizingType_UseManualOverrideTextureSize = 2,
	TextureSizingType_UseSimplygonAutomaticSizing = 3,
	TextureSizingType_MAX = 4
};

// Object: Enum Engine.ESceneTextureId
enum class ESceneTextureId : uint8_t
{
	PPI_SceneColor = 0,
	PPI_SceneDepth = 1,
	PPI_DiffuseColor = 2,
	PPI_SpecularColor = 3,
	PPI_SubsurfaceColor = 4,
	PPI_BaseColor = 5,
	PPI_Specular = 6,
	PPI_Metallic = 7,
	PPI_WorldNormal = 8,
	PPI_SeparateTranslucency = 9,
	PPI_Opacity = 10,
	PPI_Roughness = 11,
	PPI_MaterialAO = 12,
	PPI_CustomDepth = 13,
	PPI_PostProcessInput0 = 14,
	PPI_PostProcessInput1 = 15,
	PPI_PostProcessInput2 = 16,
	PPI_PostProcessInput3 = 17,
	PPI_PostProcessInput4 = 18,
	PPI_PostProcessInput5 = 19,
	PPI_PostProcessInput6 = 20,
	PPI_DecalMask = 21,
	PPI_ShadingModelColor = 22,
	PPI_ShadingModelID = 23,
	PPI_AmbientOcclusion = 24,
	PPI_CustomStencil = 25,
	PPI_StoredBaseColor = 26,
	PPI_StoredSpecular = 27,
	PPI_Velocity = 28,
	PPI_WorldTangent = 29,
	PPI_Anisotropy = 30,
	PPI_MAX = 31
};

// Object: Enum Engine.EMaterialDomain
enum class EMaterialDomain : uint8_t
{
	MD_Surface = 0,
	MD_DeferredDecal = 1,
	MD_LightFunction = 2,
	MD_Volume = 3,
	MD_PostProcess = 4,
	MD_UI = 5,
	MD_RuntimeVirtualTexture = 6,
	MD_MAX = 7
};

// Object: Enum Engine.EMeshInstancingReplacementMethod
enum class EMeshInstancingReplacementMethod : uint8_t
{
	RemoveOriginalActors = 0,
	KeepOriginalActorsAsEditorOnly = 1,
	EMeshInstancingReplacementMethod_MAX = 2
};

// Object: Enum Engine.EUVOutput
enum class EUVOutput : uint8_t
{
	DoNotOutputChannel = 0,
	OutputChannel = 1,
	EUVOutput_MAX = 2
};

// Object: Enum Engine.EMeshMergeType
enum class EMeshMergeType : uint8_t
{
	MeshMergeType_Default = 0,
	MeshMergeType_MergeActor = 1,
	MeshMergeType_MAX = 2
};

// Object: Enum Engine.EMeshLODSelectionType
enum class EMeshLODSelectionType : uint8_t
{
	AllLODs = 0,
	SpecificLOD = 1,
	CalculateLOD = 2,
	LowestDetailLOD = 3,
	EMeshLODSelectionType_MAX = 4
};

// Object: Enum Engine.EProxyNormalComputationMethod
enum class EProxyNormalComputationMethod : uint8_t
{
	AngleWeighted = 0,
	AreaWeighted = 1,
	EqualWeighted = 2,
	EProxyNormalComputationMethod_MAX = 3
};

// Object: Enum Engine.ELandscapeCullingPrecision
enum class ELandscapeCullingPrecision : uint8_t
{
	High = 0,
	Medium = 1,
	Low = 2,
	ELandscapeCullingPrecision_MAX = 3
};

// Object: Enum Engine.EStaticMeshReductionTerimationCriterion
enum class EStaticMeshReductionTerimationCriterion : uint8_t
{
	Triangles = 0,
	Vertices = 1,
	Any = 2,
	EStaticMeshReductionTerimationCriterion_MAX = 3
};

// Object: Enum Engine.EMeshFeatureImportance
enum class EMeshFeatureImportance : uint8_t
{
	Off = 0,
	Lowest = 1,
	Low = 2,
	Normal = 3,
	High = 4,
	Highest = 5,
	EMeshFeatureImportance_MAX = 6
};

// Object: Enum Engine.EVertexPaintAxis
enum class EVertexPaintAxis : uint8_t
{
	X = 0,
	Y = 1,
	Z = 2,
	EVertexPaintAxis_MAX = 3
};

// Object: Enum Engine.EMicroTransactionResult
enum class EMicroTransactionResult : uint8_t
{
	MTR_Succeeded = 0,
	MTR_Failed = 1,
	MTR_Canceled = 2,
	MTR_RestoredFromServer = 3,
	MTR_MAX = 4
};

// Object: Enum Engine.EMicroTransactionDelegate
enum class EMicroTransactionDelegate : uint8_t
{
	MTD_PurchaseQueryComplete = 0,
	MTD_PurchaseComplete = 1,
	MTD_MAX = 2
};

// Object: Enum Engine.FNavigationSystemRunMode
enum class EFNavigationSystemRunMode : uint8_t
{
	InvalidMode = 0,
	GameMode = 1,
	EditorMode = 2,
	SimulationMode = 3,
	PIEMode = 4,
	InferFromWorldMode = 5,
	FNavigationSystemRunMode_MAX = 6
};

// Object: Enum Engine.ENavigationQueryResult
enum class ENavigationQueryResult : uint8_t
{
	Invalid = 0,
	Error = 1,
	Fail = 2,
	Success = 3,
	ENavigationQueryResult_MAX = 4
};

// Object: Enum Engine.ENavPathEvent
enum class ENavPathEvent : uint8_t
{
	Cleared = 0,
	NewPath = 1,
	UpdatedDueToGoalMoved = 2,
	UpdatedDueToNavigationChanged = 3,
	Invalidated = 4,
	RePathFailed = 5,
	MetaPathUpdate = 6,
	Custom = 7,
	ENavPathEvent_MAX = 8
};

// Object: Enum Engine.ENavDataGatheringModeConfig
enum class ENavDataGatheringModeConfig : uint8_t
{
	Invalid = 0,
	Instant = 1,
	Lazy = 2,
	ENavDataGatheringModeConfig_MAX = 3
};

// Object: Enum Engine.ENavDataGatheringMode
enum class ENavDataGatheringMode : uint8_t
{
	Default = 0,
	Instant = 1,
	Lazy = 2,
	ENavDataGatheringMode_MAX = 3
};

// Object: Enum Engine.ENavigationOptionFlag
enum class ENavigationOptionFlag : uint8_t
{
	Default = 0,
	Enable = 1,
	Disable = 2,
	MAX = 3
};

// Object: Enum Engine.ENavLinkDirection
enum class ENavLinkDirection : uint8_t
{
	BothWays = 0,
	LeftToRight = 1,
	RightToLeft = 2,
	ENavLinkDirection_MAX = 3
};

// Object: Enum Engine.EEmitterRenderMode
enum class EEmitterRenderMode : uint8_t
{
	ERM_Normal = 0,
	ERM_Point = 1,
	ERM_Cross = 2,
	ERM_LightsOnly = 3,
	ERM_None = 4,
	ERM_MAX = 5
};

// Object: Enum Engine.EParticleSubUVInterpMethod
enum class EParticleSubUVInterpMethod : uint8_t
{
	PSUVIM_None = 0,
	PSUVIM_Linear = 1,
	PSUVIM_Linear_Blend = 2,
	PSUVIM_Random = 3,
	PSUVIM_Random_Blend = 4,
	PSUVIM_MAX = 5
};

// Object: Enum Engine.EParticleBurstMethod
enum class EParticleBurstMethod : uint8_t
{
	EPBM_Instant = 0,
	EPBM_Interpolated = 1,
	EPBM_MAX = 2
};

// Object: Enum Engine.EParticleSystemInsignificanceReaction
enum class EParticleSystemInsignificanceReaction : uint8_t
{
	Auto = 0,
	Complete = 1,
	DisableTick = 2,
	DisableTickAndKill = 3,
	Num = 4,
	EParticleSystemInsignificanceReaction_MAX = 5
};

// Object: Enum Engine.EParticleSignificanceLevel
enum class EParticleSignificanceLevel : uint8_t
{
	Low = 0,
	Medium = 1,
	High = 2,
	Critical = 3,
	Num = 4,
	EParticleSignificanceLevel_MAX = 5
};

// Object: Enum Engine.EParticleDetailMode
enum class EParticleDetailMode : uint8_t
{
	PDM_Low = 0,
	PDM_Medium = 1,
	PDM_High = 2,
	PDM_MAX = 3
};

// Object: Enum Engine.EParticleSourceSelectionMethod
enum class EParticleSourceSelectionMethod : uint8_t
{
	EPSSM_Random = 0,
	EPSSM_Sequential = 1,
	EPSSM_MAX = 2
};

// Object: Enum Engine.EModuleType
enum class EModuleType : uint8_t
{
	EPMT_General = 0,
	EPMT_TypeData = 1,
	EPMT_Beam = 2,
	EPMT_Trail = 3,
	EPMT_Spawn = 4,
	EPMT_Required = 5,
	EPMT_Event = 6,
	EPMT_Light = 7,
	EPMT_SubUV = 8,
	EPMT_MAX = 9
};

// Object: Enum Engine.EAttractorParticleSelectionMethod
enum class EAttractorParticleSelectionMethod : uint8_t
{
	EAPSM_Random = 0,
	EAPSM_Sequential = 1,
	EAPSM_MAX = 2
};

// Object: Enum Engine.Beam2SourceTargetTangentMethod
enum class EBeam2SourceTargetTangentMethod : uint8_t
{
	PEB2STTM_Direct = 0,
	PEB2STTM_UserSet = 1,
	PEB2STTM_Distribution = 2,
	PEB2STTM_Emitter = 3,
	PEB2STTM_MAX = 4
};

// Object: Enum Engine.Beam2SourceTargetMethod
enum class EBeam2SourceTargetMethod : uint8_t
{
	PEB2STM_Default = 0,
	PEB2STM_UserSet = 1,
	PEB2STM_Emitter = 2,
	PEB2STM_Particle = 3,
	PEB2STM_Actor = 4,
	PEB2STM_MAX = 5
};

// Object: Enum Engine.BeamModifierType
enum class EBeamModifierType : uint8_t
{
	PEB2MT_Source = 0,
	PEB2MT_Target = 1,
	PEB2MT_MAX = 2
};

// Object: Enum Engine.EParticleCameraOffsetUpdateMethod
enum class EParticleCameraOffsetUpdateMethod : uint8_t
{
	EPCOUM_DirectSet = 0,
	EPCOUM_Additive = 1,
	EPCOUM_Scalar = 2,
	EPCOUM_MAX = 3
};

// Object: Enum Engine.EParticleCollisionComplete
enum class EParticleCollisionComplete : uint8_t
{
	EPCC_Kill = 0,
	EPCC_Freeze = 1,
	EPCC_HaltCollisions = 2,
	EPCC_FreezeTranslation = 3,
	EPCC_FreezeRotation = 4,
	EPCC_FreezeMovement = 5,
	EPCC_MAX = 6
};

// Object: Enum Engine.EParticleCollisionResponse
enum class EParticleCollisionResponse : uint8_t
{
	Bounce = 0,
	Stop = 1,
	Kill = 2,
	EParticleCollisionResponse_MAX = 3
};

// Object: Enum Engine.ELocationBoneSocketSelectionMethod
enum class ELocationBoneSocketSelectionMethod : uint8_t
{
	BONESOCKETSEL_Sequential = 0,
	BONESOCKETSEL_Random = 1,
	BONESOCKETSEL_MAX = 2
};

// Object: Enum Engine.ELocationBoneSocketSource
enum class ELocationBoneSocketSource : uint8_t
{
	BONESOCKETSOURCE_Bones = 0,
	BONESOCKETSOURCE_Sockets = 1,
	BONESOCKETSOURCE_MAX = 2
};

// Object: Enum Engine.ELocationEmitterSelectionMethod
enum class ELocationEmitterSelectionMethod : uint8_t
{
	ELESM_Random = 0,
	ELESM_Sequential = 1,
	ELESM_MAX = 2
};

// Object: Enum Engine.CylinderHeightAxis
enum class ECylinderHeightAxis : uint8_t
{
	PMLPC_HEIGHTAXIS_X = 0,
	PMLPC_HEIGHTAXIS_Y = 1,
	PMLPC_HEIGHTAXIS_Z = 2,
	PMLPC_HEIGHTAXIS_MAX = 3
};

// Object: Enum Engine.ELocationSkelVertSurfaceSource
enum class ELocationSkelVertSurfaceSource : uint8_t
{
	VERTSURFACESOURCE_Vert = 0,
	VERTSURFACESOURCE_Surface = 1,
	VERTSURFACESOURCE_MAX = 2
};

// Object: Enum Engine.EOrbitChainMode
enum class EOrbitChainMode : uint8_t
{
	EOChainMode_Add = 0,
	EOChainMode_Scale = 1,
	EOChainMode_Link = 2,
	EOChainMode_MAX = 3
};

// Object: Enum Engine.EParticleAxisLock
enum class EParticleAxisLock : uint8_t
{
	EPAL_NONE = 0,
	EPAL_X = 1,
	EPAL_Y = 2,
	EPAL_Z = 3,
	EPAL_NEGATIVE_X = 4,
	EPAL_NEGATIVE_Y = 5,
	EPAL_NEGATIVE_Z = 6,
	EPAL_ROTATE_X = 7,
	EPAL_ROTATE_Y = 8,
	EPAL_ROTATE_Z = 9,
	EPAL_MAX = 10
};

// Object: Enum Engine.EEmitterDynamicParameterValue
enum class EEmitterDynamicParameterValue : uint8_t
{
	EDPV_UserSet = 0,
	EDPV_AutoSet = 1,
	EDPV_VelocityX = 2,
	EDPV_VelocityY = 3,
	EDPV_VelocityZ = 4,
	EDPV_VelocityMag = 5,
	EDPV_MAX = 6
};

// Object: Enum Engine.EEmitterNormalsMode
enum class EEmitterNormalsMode : uint8_t
{
	ENM_CameraFacing = 0,
	ENM_Spherical = 1,
	ENM_Cylindrical = 2,
	ENM_MAX = 3
};

// Object: Enum Engine.EParticleSortMode
enum class EParticleSortMode : uint8_t
{
	PSORTMODE_None = 0,
	PSORTMODE_ViewProjDepth = 1,
	PSORTMODE_DistanceToView = 2,
	PSORTMODE_Age_OldestFirst = 3,
	PSORTMODE_Age_NewestFirst = 4,
	PSORTMODE_MAX = 5
};

// Object: Enum Engine.EParticleUVFlipMode
enum class EParticleUVFlipMode : uint8_t
{
	None = 0,
	FlipUV = 1,
	FlipUOnly = 2,
	FlipVOnly = 3,
	RandomFlipUV = 4,
	RandomFlipUOnly = 5,
	RandomFlipVOnly = 6,
	RandomFlipUVIndependent = 7,
	EParticleUVFlipMode_MAX = 8
};

// Object: Enum Engine.ETrail2SourceMethod
enum class ETrail2SourceMethod : uint8_t
{
	PET2SRCM_Default = 0,
	PET2SRCM_Particle = 1,
	PET2SRCM_Actor = 2,
	PET2SRCM_MAX = 3
};

// Object: Enum Engine.EBeamTaperMethod
enum class EBeamTaperMethod : uint8_t
{
	PEBTM_None = 0,
	PEBTM_Full = 1,
	PEBTM_Partial = 2,
	PEBTM_MAX = 3
};

// Object: Enum Engine.EBeam2Method
enum class EBeam2Method : uint8_t
{
	PEB2M_Distance = 0,
	PEB2M_Target = 1,
	PEB2M_Branch = 2,
	PEB2M_MAX = 3
};

// Object: Enum Engine.EMeshCameraFacingOptions
enum class EMeshCameraFacingOptions : uint8_t
{
	XAxisFacing_NoUp = 0,
	XAxisFacing_ZUp = 1,
	XAxisFacing_NegativeZUp = 2,
	XAxisFacing_YUp = 3,
	XAxisFacing_NegativeYUp = 4,
	LockedAxis_ZAxisFacing = 5,
	LockedAxis_NegativeZAxisFacing = 6,
	LockedAxis_YAxisFacing = 7,
	LockedAxis_NegativeYAxisFacing = 8,
	VelocityAligned_ZAxisFacing = 9,
	VelocityAligned_NegativeZAxisFacing = 10,
	VelocityAligned_YAxisFacing = 11,
	VelocityAligned_NegativeYAxisFacing = 12,
	EMeshCameraFacingOptions_MAX = 13
};

// Object: Enum Engine.EMeshCameraFacingUpAxis
enum class EMeshCameraFacingUpAxis : uint8_t
{
	CameraFacing_NoneUP = 0,
	CameraFacing_ZUp = 1,
	CameraFacing_NegativeZUp = 2,
	CameraFacing_YUp = 3,
	CameraFacing_NegativeYUp = 4,
	CameraFacing_MAX = 5
};

// Object: Enum Engine.EMeshScreenAlignment
enum class EMeshScreenAlignment : uint8_t
{
	PSMA_MeshFaceCameraWithRoll = 0,
	PSMA_MeshFaceCameraWithSpin = 1,
	PSMA_MeshFaceCameraWithLockedAxis = 2,
	PSMA_MAX = 3
};

// Object: Enum Engine.ETrailsRenderAxisOption
enum class ETrailsRenderAxisOption : uint8_t
{
	Trails_CameraUp = 0,
	Trails_SourceUp = 1,
	Trails_WorldUp = 2,
	Trails_MAX = 3
};

// Object: Enum Engine.EParticleScreenAlignment
enum class EParticleScreenAlignment : uint8_t
{
	PSA_FacingCameraPosition = 0,
	PSA_Square = 1,
	PSA_Rectangle = 2,
	PSA_Velocity = 3,
	PSA_AwayFromCenter = 4,
	PSA_TypeSpecific = 5,
	PSA_FacingCameraDistanceBlend = 6,
	PSA_MAX = 7
};

// Object: Enum Engine.EParticleSystemOcclusionBoundsMethod
enum class EParticleSystemOcclusionBoundsMethod : uint8_t
{
	EPSOBM_None = 0,
	EPSOBM_ParticleBounds = 1,
	EPSOBM_CustomBounds = 2,
	EPSOBM_MAX = 3
};

// Object: Enum Engine.ParticleSystemLODMethod
enum class EParticleSystemLODMethod : uint8_t
{
	PARTICLESYSTEMLODMETHOD_Automatic = 0,
	PARTICLESYSTEMLODMETHOD_DirectSet = 1,
	PARTICLESYSTEMLODMETHOD_ActivateAutomatic = 2,
	PARTICLESYSTEMLODMETHOD_MAX = 3
};

// Object: Enum Engine.EParticleSystemUpdateMode
enum class EParticleSystemUpdateMode : uint8_t
{
	EPSUM_RealTime = 0,
	EPSUM_FixedTime = 1,
	EPSUM_MAX = 2
};

// Object: Enum Engine.EParticleEventType
enum class EParticleEventType : uint8_t
{
	EPET_Any = 0,
	EPET_Spawn = 1,
	EPET_Death = 2,
	EPET_Collision = 3,
	EPET_Burst = 4,
	EPET_Blueprint = 5,
	EPET_MAX = 6
};

// Object: Enum Engine.ParticleReplayState
enum class EParticleReplayState : uint8_t
{
	PRS_Disabled = 0,
	PRS_Capturing = 1,
	PRS_Replaying = 2,
	PRS_MAX = 3
};

// Object: Enum Engine.EParticleSysParamType
enum class EParticleSysParamType : uint8_t
{
	PSPT_None = 0,
	PSPT_Scalar = 1,
	PSPT_ScalarRand = 2,
	PSPT_Vector = 3,
	PSPT_VectorRand = 4,
	PSPT_Color = 5,
	PSPT_Actor = 6,
	PSPT_Material = 7,
	PSPT_VectorUnitRand = 8,
	PSPT_MAX = 9
};

// Object: Enum Engine.ESettingsLockedAxis
enum class ESettingsLockedAxis : uint8_t
{
	None = 0,
	X = 1,
	Y = 2,
	Z = 3,
	Invalid = 4,
	ESettingsLockedAxis_MAX = 5
};

// Object: Enum Engine.ESettingsDOF
enum class ESettingsDOF : uint8_t
{
	Full3D = 0,
	YZPlane = 1,
	XZPlane = 2,
	XYPlane = 3,
	ESettingsDOF_MAX = 4
};

// Object: Enum Engine.ERendererStencilMask
enum class ERendererStencilMask : uint8_t
{
	ERSM_Default = 0,
	ERSM = 1,
	ERSM = 2,
	ERSM = 3,
	ERSM = 4,
	ERSM = 5,
	ERSM = 6,
	ERSM = 7,
	ERSM = 8,
	ERSM = 9,
	ERSM_MAX = 10
};

// Object: Enum Engine.EHasCustomNavigableGeometry
enum class EHasCustomNavigableGeometry : uint8_t
{
	No = 0,
	Yes = 1,
	EvenIfNotCollidable = 2,
	DontExport = 3,
	EHasCustomNavigableGeometry_MAX = 4
};

// Object: Enum Engine.EMobileStencilSetting
enum class EMobileStencilSetting : uint8_t
{
	EMSV_NoCut = 0,
	EMSV_WithSilhouette = 1,
	EMSV_MAX = 2
};

// Object: Enum Engine.ECanBeCharacterBase
enum class ECanBeCharacterBase : uint8_t
{
	ECB_No = 0,
	ECB_Yes = 1,
	ECB_Owner = 2,
	ECB_MAX = 3
};

// Object: Enum Engine.EQuarztQuantizationReference
enum class EQuarztQuantizationReference : uint8_t
{
	BarRelative = 0,
	TransportRelative = 1,
	CurrentTimeRelative = 2,
	Count = 3,
	EQuarztQuantizationReference_MAX = 4
};

// Object: Enum Engine.EQuartzDelegateType
enum class EQuartzDelegateType : uint8_t
{
	MetronomeTick = 0,
	CommandEvent = 1,
	Count = 2,
	EQuartzDelegateType_MAX = 3
};

// Object: Enum Engine.EQuartzTimeSignatureQuantization
enum class EQuartzTimeSignatureQuantization : uint8_t
{
	HalfNote = 0,
	QuarterNote = 1,
	EighthNote = 2,
	SixteenthNote = 3,
	ThirtySecondNote = 4,
	Count = 5,
	EQuartzTimeSignatureQuantization_MAX = 6
};

// Object: Enum Engine.EMobileReflectionCompression
enum class EMobileReflectionCompression : uint8_t
{
	Default = 0,
	On = 1,
	Off = 2,
	EMobileReflectionCompression_MAX = 3
};

// Object: Enum Engine.EReflectionSourceType
enum class EReflectionSourceType : uint8_t
{
	CapturedScene = 0,
	SpecifiedCubemap = 1,
	EReflectionSourceType_MAX = 2
};

// Object: Enum Engine.ERegionalEffectsType
enum class ERegionalEffectsType : uint8_t
{
	RET_ComponentProperty = 0,
	RET_MaterialParameter = 1,
	RET_PostProcessParameter = 2,
	RET_MAX = 3
};

// Object: Enum Engine.ERegionalEffectsValueType
enum class ERegionalEffectsValueType : uint8_t
{
	REVT_FixedFloat = 0,
	REVT_FixedColor = 1,
	REVT_Curve = 2,
	REVT_MAX = 3
};

// Object: Enum Engine.ETODCurveTimeType
enum class ETODCurveTimeType : uint8_t
{
	TODCTT_SunZ = 0,
	TODCTT_TimeOfDay = 1,
	TODCTT_InternalTimeOfDay = 2,
	TODCTT_MAX = 3
};

// Object: Enum Engine.EDefaultBackBufferPixelFormat
enum class EDefaultBackBufferPixelFormat : uint8_t
{
	DBBPF_B8G8R8A8 = 0,
	DBBPF_A16B16G16R16_DEPRECATED = 1,
	DBBPF_FloatRGB_DEPRECATED = 2,
	DBBPF_FloatRGBA = 3,
	DBBPF_A2B10G10R10 = 4,
	DBBPF_MAX = 5
};

// Object: Enum Engine.EAutoExposureMethodUI
enum class EAutoExposureMethodUI : uint8_t
{
	AEM_Histogram = 0,
	AEM_Basic = 1,
	AEM_Manual = 2,
	AEM_MAX = 3
};

// Object: Enum Engine.EAlphaChannelMode
enum class EAlphaChannelMode : uint8_t
{
	Disabled = 0,
	LinearColorSpaceOnly = 1,
	AllowThroughTonemapper = 2,
	EAlphaChannelMode_MAX = 3
};

// Object: Enum Engine.EEarlyZPass
enum class EEarlyZPass : uint8_t
{
	None = 0,
	OpaqueOnly = 1,
	OpaqueAndMasked = 2,
	Auto = 3,
	EEarlyZPass_MAX = 4
};

// Object: Enum Engine.ECustomDepthStencil
enum class ECustomDepthStencil : uint8_t
{
	Disabled = 0,
	Enabled = 1,
	EnabledOnDemand = 2,
	EnabledWithStencil = 3,
	ECustomDepthStencil_MAX = 4
};

// Object: Enum Engine.EMobileMSAASampleCount
enum class EMobileMSAASampleCount : uint8_t
{
	One = 1,
	Two = 2,
	Four = 4,
	Eight = 8,
	EMobileMSAASampleCount_MAX = 9
};

// Object: Enum Engine.ECompositingSampleCount
enum class ECompositingSampleCount : uint8_t
{
	One = 1,
	Two = 2,
	Four = 4,
	Eight = 8,
	ECompositingSampleCount_MAX = 9
};

// Object: Enum Engine.EClearSceneOptions
enum class EClearSceneOptions : uint8_t
{
	NoClear = 0,
	HardwareClear = 1,
	QuadAtMaxZ = 2,
	EClearSceneOptions_MAX = 3
};

// Object: Enum Engine.EReporterLineStyle
enum class EReporterLineStyle : uint8_t
{
	Line = 0,
	Dash = 1,
	EReporterLineStyle_MAX = 2
};

// Object: Enum Engine.ELegendPosition
enum class ELegendPosition : uint8_t
{
	Outside = 0,
	Inside = 1,
	ELegendPosition_MAX = 2
};

// Object: Enum Engine.EGraphDataStyle
enum class EGraphDataStyle : uint8_t
{
	Lines = 0,
	Filled = 1,
	EGraphDataStyle_MAX = 2
};

// Object: Enum Engine.EGraphAxisStyle
enum class EGraphAxisStyle : uint8_t
{
	Lines = 0,
	Notches = 1,
	Grid = 2,
	EGraphAxisStyle_MAX = 3
};

// Object: Enum Engine.ReverbPreset
enum class EReverbPreset : uint8_t
{
	REVERB_Default = 0,
	REVERB_Bathroom = 1,
	REVERB_StoneRoom = 2,
	REVERB_Auditorium = 3,
	REVERB_ConcertHall = 4,
	REVERB_Cave = 5,
	REVERB_Hallway = 6,
	REVERB_StoneCorridor = 7,
	REVERB_Alley = 8,
	REVERB_Forest = 9,
	REVERB_City = 10,
	REVERB_Mountains = 11,
	REVERB_Quarry = 12,
	REVERB_Plain = 13,
	REVERB_ParkingLot = 14,
	REVERB_SewerPipe = 15,
	REVERB_Underwater = 16,
	REVERB_SmallRoom = 17,
	REVERB_MediumRoom = 18,
	REVERB_LargeRoom = 19,
	REVERB_MediumHall = 20,
	REVERB_LargeHall = 21,
	REVERB_Plate = 22,
	REVERB_MAX = 23
};

// Object: Enum Engine.ERichCurveKeyTimeCompressionFormat
enum class ERichCurveKeyTimeCompressionFormat : uint8_t
{
	RCKTCF_uint16 = 0,
	RCKTCF_float32 = 1,
	RCKTCF_MAX = 2
};

// Object: Enum Engine.ERichCurveCompressionFormat
enum class ERichCurveCompressionFormat : uint8_t
{
	RCCF_Empty = 0,
	RCCF_Constant = 1,
	RCCF_Linear = 2,
	RCCF_Cubic = 3,
	RCCF_Mixed = 4,
	RCCF_Weighted = 5,
	RCCF_MAX = 6
};

// Object: Enum Engine.EConstraintTransform
enum class EConstraintTransform : uint8_t
{
	Absolute = 0,
	Relative = 1,
	EConstraintTransform_MAX = 2
};

// Object: Enum Engine.EControlConstraint
enum class EControlConstraint : uint8_t
{
	Orientation = 0,
	Translation = 1,
	MAX = 2
};

// Object: Enum Engine.ERootMotionFinishVelocityMode
enum class ERootMotionFinishVelocityMode : uint8_t
{
	MaintainLastRootMotionVelocity = 0,
	SetVelocity = 1,
	ClampVelocity = 2,
	ERootMotionFinishVelocityMode_MAX = 3
};

// Object: Enum Engine.ERootMotionSourceSettingsFlags
enum class ERootMotionSourceSettingsFlags : uint8_t
{
	UseSensitiveLiftoffCheck = 1,
	DisablePartialEndTick = 2,
	IgnoreZAccumulate = 4,
	ERootMotionSourceSettingsFlags_MAX = 5
};

// Object: Enum Engine.ERootMotionSourceStatusFlags
enum class ERootMotionSourceStatusFlags : uint8_t
{
	Prepared = 1,
	Finished = 2,
	MarkedForRemoval = 4,
	ERootMotionSourceStatusFlags_MAX = 5
};

// Object: Enum Engine.ERootMotionAccumulateMode
enum class ERootMotionAccumulateMode : uint8_t
{
	Override = 0,
	Additive = 1,
	ERootMotionAccumulateMode_MAX = 2
};

// Object: Enum Engine.ERuntimeVirtualTextureMainPassType
enum class ERuntimeVirtualTextureMainPassType : uint8_t
{
	Never = 0,
	Exclusive = 1,
	Always = 2,
	ERuntimeVirtualTextureMainPassType_MAX = 3
};

// Object: Enum Engine.ERuntimeVirtualTextureMaterialType
enum class ERuntimeVirtualTextureMaterialType : uint8_t
{
	BaseColor = 0,
	BaseColor_Normal_DEPRECATED = 1,
	BaseColor_Normal_Specular = 2,
	BaseColor_Normal_Specular_YCoCg = 3,
	BaseColor_Normal_Specular_Mask_YCoCg = 4,
	WorldHeight = 5,
	Count = 6,
	ERuntimeVirtualTextureMaterialType_MAX = 7
};

// Object: Enum Engine.EMobilePixelProjectedReflectionQuality
enum class EMobilePixelProjectedReflectionQuality : uint8_t
{
	Disabled = 0,
	BestPerformance = 1,
	BetterQuality = 2,
	BestQuality = 3,
	EMobilePixelProjectedReflectionQuality_MAX = 4
};

// Object: Enum Engine.EMobilePlanarReflectionMode
enum class EMobilePlanarReflectionMode : uint8_t
{
	Usual = 0,
	MobilePPRExclusive = 1,
	MobilePPR = 2,
	EMobilePlanarReflectionMode_MAX = 3
};

// Object: Enum Engine.EReflectedAndRefractedRayTracedShadows
enum class EReflectedAndRefractedRayTracedShadows : uint8_t
{
	Disabled = 0,
	Hard_shadows = 1,
	Area_shadows = 2,
	EReflectedAndRefractedRayTracedShadows_MAX = 3
};

// Object: Enum Engine.ERayTracingGlobalIlluminationType
enum class ERayTracingGlobalIlluminationType : uint8_t
{
	Disabled = 0,
	BruteForce = 1,
	FinalGather = 2,
	ERayTracingGlobalIlluminationType_MAX = 3
};

// Object: Enum Engine.ETranslucencyType
enum class ETranslucencyType : uint8_t
{
	Raster = 0,
	RayTracing = 1,
	ETranslucencyType_MAX = 2
};

// Object: Enum Engine.EReflectionsType
enum class EReflectionsType : uint8_t
{
	ScreenSpace = 0,
	RayTracing = 1,
	EReflectionsType_MAX = 2
};

// Object: Enum Engine.ELightUnits
enum class ELightUnits : uint8_t
{
	Unitless = 0,
	Candelas = 1,
	Lumens = 2,
	ELightUnits_MAX = 3
};

// Object: Enum Engine.EBloomMethod
enum class EBloomMethod : uint8_t
{
	BM_SOG = 0,
	BM_FFT = 1,
	BM_MAX = 2
};

// Object: Enum Engine.EAutoExposureMethod
enum class EAutoExposureMethod : uint8_t
{
	AEM_Histogram = 0,
	AEM_Basic = 1,
	AEM_Manual = 2,
	AEM_MAX = 3
};

// Object: Enum Engine.EAntiAliasingMethod
enum class EAntiAliasingMethod : uint8_t
{
	AAM_None = 0,
	AAM_FXAA = 1,
	AAM_TemporalAA = 2,
	AAM_SMAA = 3,
	AAM_MSAA = 4,
	AAM_MAX = 5
};

// Object: Enum Engine.EDepthOfFieldMethod
enum class EDepthOfFieldMethod : uint8_t
{
	DOFM_BokehDOF = 0,
	DOFM_Gaussian = 1,
	DOFM_CircleDOF = 2,
	DOFM_MAX = 3
};

// Object: Enum Engine.ESceneCapturePrimitiveRenderMode
enum class ESceneCapturePrimitiveRenderMode : uint8_t
{
	PRM_LegacySceneCapture = 0,
	PRM_RenderScenePrimitives = 1,
	PRM_UseShowOnlyList = 2,
	PRM_MAX = 3
};

// Object: Enum Engine.EMaterialProperty
enum class EMaterialProperty : uint8_t
{
	MP_EmissiveColor = 0,
	MP_Opacity = 1,
	MP_OpacityMask = 2,
	MP_DiffuseColor = 3,
	MP_SpecularColor = 4,
	MP_BaseColor = 5,
	MP_Metallic = 6,
	MP_Specular = 7,
	MP_Roughness = 8,
	MP_Anisotropy = 9,
	MP_Normal = 10,
	MP_Tangent = 11,
	MP_WorldPositionOffset = 12,
	MP_WorldDisplacement = 13,
	MP_TessellationMultiplier = 14,
	MP_SubsurfaceColor = 15,
	MP_CustomData0 = 16,
	MP_CustomData1 = 17,
	MP_AmbientOcclusion = 18,
	MP_Refraction = 19,
	MP_CustomizedUVs0 = 20,
	MP_CustomizedUVs1 = 21,
	MP_CustomizedUVs2 = 22,
	MP_CustomizedUVs3 = 23,
	MP_CustomizedUVs4 = 24,
	MP_CustomizedUVs5 = 25,
	MP_CustomizedUVs6 = 26,
	MP_CustomizedUVs7 = 27,
	MP_PixelDepthOffset = 28,
	MP_ShadingModel = 29,
	MP_CustomData2 = 30,
	MP_CustomData3 = 31,
	MP_CustomData4 = 32,
	MP_CustomData5 = 33,
	MP_CustomData6 = 34,
	MP_CustomData7 = 35,
	MP_IblIntensity = 36,
	MP_DirLightDecay = 37,
	MP_Porosity = 38,
	MP_CustomGI = 39,
	MP_AmbientOcclusionIntensity = 40,
	MP_DarkFogEmissive = 41,
	MP_MaterialAttributes = 42,
	MP_CustomOutput = 43,
	MP_MAX = 44
};

// Object: Enum Engine.ESkinCacheDefaultBehavior
enum class ESkinCacheDefaultBehavior : uint8_t
{
	Exclusive = 0,
	Inclusive = 1,
	ESkinCacheDefaultBehavior_MAX = 2
};

// Object: Enum Engine.ESkinCacheUsage
enum class ESkinCacheUsage : uint8_t
{
	Auto = 0,
	Disabled = 1,
	Enabled = 2,
	ESkinCacheUsage_MAX = 3
};

// Object: Enum Engine.EBoneRetargetingMode
enum class EBoneRetargetingMode : uint8_t
{
	Animation = 0,
	Skeleton = 1,
	AnimationScaled = 2,
	AnimationRelative = 3,
	OrientAndScale = 4,
	EBoneRetargetingMode_MAX = 5
};

// Object: Enum Engine.EPhysicsTransformUpdateMode
enum class EPhysicsTransformUpdateMode : uint8_t
{
	SimulationUpatesComponentTransform = 0,
	ComponentTransformIsKinematic = 1,
	EPhysicsTransformUpdateMode_MAX = 2
};

// Object: Enum Engine.EAnimationMode
enum class EAnimationMode : uint8_t
{
	AnimationBlueprint = 0,
	AnimationSingleNode = 1,
	AnimationCustomMode = 2,
	EAnimationMode_MAX = 3
};

// Object: Enum Engine.EKinematicBonesUpdateToPhysics
enum class EKinematicBonesUpdateToPhysics : uint8_t
{
	SkipSimulatingBones = 0,
	SkipAllBones = 1,
	EKinematicBonesUpdateToPhysics_MAX = 2
};

// Object: Enum Engine.ECustomBoneAttributeLookup
enum class ECustomBoneAttributeLookup : uint8_t
{
	BoneOnly = 0,
	ImmediateParent = 1,
	ParentHierarchy = 2,
	ECustomBoneAttributeLookup_MAX = 3
};

// Object: Enum Engine.EClothMassMode
enum class EClothMassMode : uint8_t
{
	UniformMass = 0,
	TotalMass = 1,
	Density = 2,
	MaxClothMassMode = 3,
	EClothMassMode_MAX = 4
};

// Object: Enum Engine.EAnimCurveType
enum class EAnimCurveType : uint8_t
{
	AttributeCurve = 0,
	MaterialCurve = 1,
	MorphTargetCurve = 2,
	MaxAnimCurveType = 3,
	EAnimCurveType_MAX = 4
};

// Object: Enum Engine.ESkeletalMeshSkinningImportVersions
enum class ESkeletalMeshSkinningImportVersions : uint8_t
{
	Before_Versionning = 0,
	SkeletalMeshBuildRefactor = 1,
	VersionPlusOne = 2,
	LatestVersion = 3,
	ESkeletalMeshSkinningImportVersions_MAX = 4
};

// Object: Enum Engine.ESkeletalMeshGeoImportVersions
enum class ESkeletalMeshGeoImportVersions : uint8_t
{
	Before_Versionning = 0,
	SkeletalMeshBuildRefactor = 1,
	VersionPlusOne = 2,
	LatestVersion = 3,
	ESkeletalMeshGeoImportVersions_MAX = 4
};

// Object: Enum Engine.EBoneFilterActionOption
enum class EBoneFilterActionOption : uint8_t
{
	Remove = 0,
	Keep = 1,
	Invalid = 2,
	EBoneFilterActionOption_MAX = 3
};

// Object: Enum Engine.SkeletalMeshOptimizationImportance
enum class ESkeletalMeshOptimizationImportance : uint8_t
{
	SMOI_Off = 0,
	SMOI_Lowest = 1,
	SMOI_Low = 2,
	SMOI_Normal = 3,
	SMOI_High = 4,
	SMOI_Highest = 5,
	SMOI_MAX = 6
};

// Object: Enum Engine.SkeletalMeshOptimizationType
enum class ESkeletalMeshOptimizationType : uint8_t
{
	SMOT_NumOfTriangles = 0,
	SMOT_MaxDeviation = 1,
	SMOT_TriangleOrDeviation = 2,
	SMOT_MAX = 3
};

// Object: Enum Engine.SkeletalMeshTerminationCriterion
enum class ESkeletalMeshTerminationCriterion : uint8_t
{
	SMTC_NumOfTriangles = 0,
	SMTC_NumOfVerts = 1,
	SMTC_TriangleOrVert = 2,
	SMTC_AbsNumOfTriangles = 3,
	SMTC_AbsNumOfVerts = 4,
	SMTC_AbsTriangleOrVert = 5,
	SMTC_MAX = 6
};

// Object: Enum Engine.EBoneTranslationRetargetingMode
enum class EBoneTranslationRetargetingMode : uint8_t
{
	Animation = 0,
	Skeleton = 1,
	AnimationScaled = 2,
	AnimationRelative = 3,
	OrientAndScale = 4,
	CustomAnimationRelative = 5,
	EBoneTranslationRetargetingMode_MAX = 6
};

// Object: Enum Engine.EVertexOffsetUsageType
enum class EVertexOffsetUsageType : uint8_t
{
	None = 0,
	PreSkinningOffset = 1,
	PostSkinningOffset = 2,
	EVertexOffsetUsageType_MAX = 3
};

// Object: Enum Engine.EVisibilityBasedAnimTickOption
enum class EVisibilityBasedAnimTickOption : uint8_t
{
	AlwaysTickPoseAndRefreshBones = 0,
	AlwaysTickPose = 1,
	OnlyTickMontagesWhenNotRendered = 2,
	OnlyTickPoseWhenRendered = 3,
	EVisibilityBasedAnimTickOption_MAX = 4
};

// Object: Enum Engine.EPhysBodyOp
enum class EPhysBodyOp : uint8_t
{
	PBO_None = 0,
	PBO_Term = 1,
	PBO_MAX = 2
};

// Object: Enum Engine.EBoneVisibilityStatus
enum class EBoneVisibilityStatus : uint8_t
{
	BVS_HiddenByParent = 0,
	BVS_Visible = 1,
	BVS_ExplicitlyHidden = 2,
	BVS_HideSelfOnly = 3,
	BVS_MAX = 4
};

// Object: Enum Engine.ESkyAtmosphereTransformMode
enum class ESkyAtmosphereTransformMode : uint8_t
{
	PlanetTopAtAbsoluteWorldOrigin = 0,
	PlanetTopAtComponentTransform = 1,
	PlanetCenterAtComponentTransform = 2,
	ESkyAtmosphereTransformMode_MAX = 3
};

// Object: Enum Engine.ESkyLightLowHemisphereColorMode
enum class ESkyLightLowHemisphereColorMode : uint8_t
{
	SLHCM_Hemisphere = 0,
	SLHCM_NegZ = 1,
	SLHCM_MAX = 2
};

// Object: Enum Engine.ESkyLightSourceType
enum class ESkyLightSourceType : uint8_t
{
	SLS_CapturedScene = 0,
	SLS_SpecifiedCubemap = 1,
	SLS_MAX = 2
};

// Object: Enum Engine.EPriorityAttenuationMethod
enum class EPriorityAttenuationMethod : uint8_t
{
	Linear = 0,
	CustomCurve = 1,
	Manual = 2,
	EPriorityAttenuationMethod_MAX = 3
};

// Object: Enum Engine.ESubmixSendMethod
enum class ESubmixSendMethod : uint8_t
{
	Linear = 0,
	CustomCurve = 1,
	Manual = 2,
	ESubmixSendMethod_MAX = 3
};

// Object: Enum Engine.EReverbSendMethod
enum class EReverbSendMethod : uint8_t
{
	Linear = 0,
	CustomCurve = 1,
	Manual = 2,
	EReverbSendMethod_MAX = 3
};

// Object: Enum Engine.EAirAbsorptionMethod
enum class EAirAbsorptionMethod : uint8_t
{
	Linear = 0,
	CustomCurve = 1,
	EAirAbsorptionMethod_MAX = 2
};

// Object: Enum Engine.ESoundSpatializationAlgorithm
enum class ESoundSpatializationAlgorithm : uint8_t
{
	SPATIALIZATION_Default = 0,
	SPATIALIZATION_HRTF = 1,
	SPATIALIZATION_MAX = 2
};

// Object: Enum Engine.ESoundDistanceCalc
enum class ESoundDistanceCalc : uint8_t
{
	SOUNDDISTANCE_Normal = 0,
	SOUNDDISTANCE_InfiniteXYPlane = 1,
	SOUNDDISTANCE_InfiniteXZPlane = 2,
	SOUNDDISTANCE_InfiniteYZPlane = 3,
	SOUNDDISTANCE_MAX = 4
};

// Object: Enum Engine.EVirtualizationMode
enum class EVirtualizationMode : uint8_t
{
	Disabled = 0,
	PlayWhenSilent = 1,
	Restart = 2,
	EVirtualizationMode_MAX = 3
};

// Object: Enum Engine.EConcurrencyVolumeScaleMode
enum class EConcurrencyVolumeScaleMode : uint8_t
{
	Default = 0,
	Distance = 1,
	Priority = 2,
	EConcurrencyVolumeScaleMode_MAX = 3
};

// Object: Enum Engine.EMaxConcurrentResolutionRule
enum class EMaxConcurrentResolutionRule : uint8_t
{
	PreventNew = 0,
	StopOldest = 1,
	StopFarthestThenPreventNew = 2,
	StopFarthestThenOldest = 3,
	StopLowestPriority = 4,
	StopQuietest = 5,
	StopLowestPriorityThenPreventNew = 6,
	Count = 7,
	EMaxConcurrentResolutionRule_MAX = 8
};

// Object: Enum Engine.ESoundGroup
enum class ESoundGroup : uint8_t
{
	SOUNDGROUP_Default = 0,
	SOUNDGROUP_Effects = 1,
	SOUNDGROUP_UI = 2,
	SOUNDGROUP_Music = 3,
	SOUNDGROUP_Voice = 4,
	SOUNDGROUP_GameSoundGroup1 = 5,
	SOUNDGROUP_GameSoundGroup2 = 6,
	SOUNDGROUP_GameSoundGroup3 = 7,
	SOUNDGROUP_GameSoundGroup4 = 8,
	SOUNDGROUP_GameSoundGroup5 = 9,
	SOUNDGROUP_GameSoundGroup6 = 10,
	SOUNDGROUP_GameSoundGroup7 = 11,
	SOUNDGROUP_GameSoundGroup8 = 12,
	SOUNDGROUP_GameSoundGroup9 = 13,
	SOUNDGROUP_GameSoundGroup10 = 14,
	SOUNDGROUP_GameSoundGroup11 = 15,
	SOUNDGROUP_GameSoundGroup12 = 16,
	SOUNDGROUP_GameSoundGroup13 = 17,
	SOUNDGROUP_GameSoundGroup14 = 18,
	SOUNDGROUP_GameSoundGroup15 = 19,
	SOUNDGROUP_GameSoundGroup16 = 20,
	SOUNDGROUP_GameSoundGroup17 = 21,
	SOUNDGROUP_GameSoundGroup18 = 22,
	SOUNDGROUP_GameSoundGroup19 = 23,
	SOUNDGROUP_GameSoundGroup20 = 24,
	SOUNDGROUP_MAX = 25
};

// Object: Enum Engine.EModulationRouting
enum class EModulationRouting : uint8_t
{
	Disable = 0,
	Inherit = 1,
	Override = 2,
	EModulationRouting_MAX = 3
};

// Object: Enum Engine.ModulationParamMode
enum class EModulationParamMode : uint8_t
{
	MPM_Normal = 0,
	MPM_Abs = 1,
	MPM_Direct = 2,
	MPM_MAX = 3
};

// Object: Enum Engine.ESourceBusChannels
enum class ESourceBusChannels : uint8_t
{
	Mono = 0,
	Stereo = 1,
	ESourceBusChannels_MAX = 2
};

// Object: Enum Engine.ESourceBusSendLevelControlMethod
enum class ESourceBusSendLevelControlMethod : uint8_t
{
	Linear = 0,
	CustomCurve = 1,
	Manual = 2,
	ESourceBusSendLevelControlMethod_MAX = 3
};

// Object: Enum Engine.EGainParamMode
enum class EGainParamMode : uint8_t
{
	Linear = 0,
	Decibels = 1,
	EGainParamMode_MAX = 2
};

// Object: Enum Engine.EAudioSpectrumType
enum class EAudioSpectrumType : uint8_t
{
	MagnitudeSpectrum = 0,
	PowerSpectrum = 1,
	Decibel = 2,
	EAudioSpectrumType_MAX = 3
};

// Object: Enum Engine.EFFTWindowType
enum class EFFTWindowType : uint8_t
{
	None = 0,
	Hamming = 1,
	Hann = 2,
	Blackman = 3,
	EFFTWindowType_MAX = 4
};

// Object: Enum Engine.EFFTPeakInterpolationMethod
enum class EFFTPeakInterpolationMethod : uint8_t
{
	NearestNeighbor = 0,
	Linear = 1,
	Quadratic = 2,
	ConstantQ = 3,
	EFFTPeakInterpolationMethod_MAX = 4
};

// Object: Enum Engine.EFFTSize
enum class EFFTSize : uint8_t
{
	DefaultSize = 0,
	Min = 1,
	Small = 2,
	Medium = 3,
	Large = 4,
	VeryLarge = 5,
	Max = 6
};

// Object: Enum Engine.ESubmixSendStage
enum class ESubmixSendStage : uint8_t
{
	PostDistanceAttenuation = 0,
	PreDistanceAttenuation = 1,
	ESubmixSendStage_MAX = 2
};

// Object: Enum Engine.ESendLevelControlMethod
enum class ESendLevelControlMethod : uint8_t
{
	Linear = 0,
	CustomCurve = 1,
	Manual = 2,
	ESendLevelControlMethod_MAX = 3
};

// Object: Enum Engine.EAudioRecordingExportType
enum class EAudioRecordingExportType : uint8_t
{
	SoundWave = 0,
	WavFile = 1,
	EAudioRecordingExportType_MAX = 2
};

// Object: Enum Engine.EAudioSpectrumBandPresetType
enum class EAudioSpectrumBandPresetType : uint8_t
{
	KickDrum = 0,
	SnareDrum = 1,
	Voice = 2,
	Cymbals = 3,
	EAudioSpectrumBandPresetType_MAX = 4
};

// Object: Enum Engine.ESoundWaveFFTSize
enum class ESoundWaveFFTSize : uint8_t
{
	VerySmall = 0,
	Small = 1,
	Medium = 2,
	Large = 3,
	VeryLarge = 4,
	ESoundWaveFFTSize_MAX = 5
};

// Object: Enum Engine.EDecompressionType
enum class EDecompressionType : uint8_t
{
	DTYPE_Setup = 0,
	DTYPE_Invalid = 1,
	DTYPE_Preview = 2,
	DTYPE_Native = 3,
	DTYPE_RealTime = 4,
	DTYPE_Procedural = 5,
	DTYPE_Xenon = 6,
	DTYPE_Streaming = 7,
	DTYPE_MAX = 8
};

// Object: Enum Engine.ESoundWaveLoadingBehavior
enum class ESoundWaveLoadingBehavior : uint16_t
{
	Inherited = 0,
	RetainOnLoad = 1,
	PrimeOnLoad = 2,
	LoadOnDemand = 3,
	ForceInline = 4,
	Uninitialized = 255,
	ESoundWaveLoadingBehavior_MAX = 256
};

// Object: Enum Engine.ESplineCoordinateSpace
enum class ESplineCoordinateSpace : uint8_t
{
	Local = 0,
	World = 1,
	ESplineCoordinateSpace_MAX = 2
};

// Object: Enum Engine.ESplinePointType
enum class ESplinePointType : uint8_t
{
	Linear = 0,
	Curve = 1,
	Constant = 2,
	CurveClamped = 3,
	CurveCustomTangent = 4,
	ESplinePointType_MAX = 5
};

// Object: Enum Engine.ESplineMeshAxis
enum class ESplineMeshAxis : uint8_t
{
	X = 0,
	Y = 1,
	Z = 2,
	ESplineMeshAxis_MAX = 3
};

// Object: Enum Engine.EAssetMeshType
enum class EAssetMeshType : uint8_t
{
	None = 0,
	Tree = 1,
	Grass = 2,
	Bush = 3,
	EAssetMeshType_MAX = 4
};

// Object: Enum Engine.EOptimizationType
enum class EOptimizationType : uint8_t
{
	OT_NumOfTriangles = 0,
	OT_MaxDeviation = 1,
	OT_MAX = 2
};

// Object: Enum Engine.EImportanceLevel
enum class EImportanceLevel : uint8_t
{
	IL_Off = 0,
	IL_Lowest = 1,
	IL_Low = 2,
	IL_Normal = 3,
	IL_High = 4,
	IL_Highest = 5,
	TEMP_BROKEN2 = 6,
	EImportanceLevel_MAX = 7
};

// Object: Enum Engine.ENormalMode
enum class ENormalMode : uint8_t
{
	NM_PreserveSmoothingGroups = 0,
	NM_RecalculateNormals = 1,
	NM_RecalculateNormalsSmooth = 2,
	NM_RecalculateNormalsHard = 3,
	TEMP_BROKEN = 4,
	ENormalMode_MAX = 5
};

// Object: Enum Engine.EHlodLevelType
enum class EHlodLevelType : uint8_t
{
	None = 0,
	Building = 1,
	Rock = 2,
	Landmark = 4,
	FarRock = 8,
	EHlodLevelType_MAX = 9
};

// Object: Enum Engine.EStereoLayerShape
enum class EStereoLayerShape : uint8_t
{
	SLSH_QuadLayer = 0,
	SLSH_CylinderLayer = 1,
	SLSH_CubemapLayer = 2,
	SLSH_EquirectLayer = 3,
	SLSH_MAX = 4
};

// Object: Enum Engine.EStereoLayerType
enum class EStereoLayerType : uint8_t
{
	SLT_WorldLocked = 0,
	SLT_TrackerLocked = 1,
	SLT_FaceLocked = 2,
	SLT_MAX = 3
};

// Object: Enum Engine.EOpacitySourceMode
enum class EOpacitySourceMode : uint8_t
{
	OSM_Alpha = 0,
	OSM_ColorBrightness = 1,
	OSM_RedChannel = 2,
	OSM_GreenChannel = 3,
	OSM_BlueChannel = 4,
	OSM_MAX = 5
};

// Object: Enum Engine.ESubUVBoundingVertexCount
enum class ESubUVBoundingVertexCount : uint8_t
{
	BVC_FourVertices = 0,
	BVC_EightVertices = 1,
	BVC_MAX = 2
};

// Object: Enum Engine.EVerticalTextAligment
enum class EVerticalTextAligment : uint8_t
{
	EVRTA_TextTop = 0,
	EVRTA_TextCenter = 1,
	EVRTA_TextBottom = 2,
	EVRTA_QuadTop = 3,
	EVRTA_MAX = 4
};

// Object: Enum Engine.EHorizTextAligment
enum class EHorizTextAligment : uint8_t
{
	EHTA_Left = 0,
	EHTA_Center = 1,
	EHTA_Right = 2,
	EHTA_MAX = 3
};

// Object: Enum Engine.ETextureLossyCompressionAmount
enum class ETextureLossyCompressionAmount : uint8_t
{
	TLCA_Default = 0,
	TLCA_None = 1,
	TLCA_Lowest = 2,
	TLCA_Low = 3,
	TLCA_Medium = 4,
	TLCA_High = 5,
	TLCA_Highest = 6,
	TLCA_MAX = 7
};

// Object: Enum Engine.ECompressedInBasePackRatio
enum class ECompressedInBasePackRatio : uint8_t
{
	NotCompress = 0,
	OneFourth = 1,
	OneSixteenth = 2,
	ECompressedInBasePackRatio_MAX = 3
};

// Object: Enum Engine.ETextureCompressionQuality
enum class ETextureCompressionQuality : uint8_t
{
	TCQ_Default = 0,
	TCQ_Lowest = 1,
	TCQ_Low = 2,
	TCQ_Medium = 3,
	TCQ_High = 4,
	TCQ_Highest = 5,
	TCQ_MAX = 6
};

// Object: Enum Engine.ETextureSourceFormat
enum class ETextureSourceFormat : uint8_t
{
	TSF_Invalid = 0,
	TSF_G8 = 1,
	TSF_BGRA8 = 2,
	TSF_BGRE8 = 3,
	TSF_RGBA16 = 4,
	TSF_RGBA16F = 5,
	TSF_RGBA8 = 6,
	TSF_RGBE8 = 7,
	TSF_G16 = 8,
	TSF_MAX = 9
};

// Object: Enum Engine.ETextureSourceArtType
enum class ETextureSourceArtType : uint8_t
{
	TSAT_Uncompressed = 0,
	TSAT_PNGCompressed = 1,
	TSAT_DDSFile = 2,
	TSAT_MAX = 3
};

// Object: Enum Engine.ETextureMipCount
enum class ETextureMipCount : uint8_t
{
	TMC_ResidentMips = 0,
	TMC_AllMips = 1,
	TMC_AllMipsBiased = 2,
	TMC_MAX = 3
};

// Object: Enum Engine.ECompositeTextureMode
enum class ECompositeTextureMode : uint8_t
{
	CTM_Disabled = 0,
	CTM_NormalRoughnessToRed = 1,
	CTM_NormalRoughnessToGreen = 2,
	CTM_NormalRoughnessToBlue = 3,
	CTM_NormalRoughnessToAlpha = 4,
	CTM_MAX = 5
};

// Object: Enum Engine.TextureAddress
enum class ETextureAddress : uint8_t
{
	TA_Wrap = 0,
	TA_Clamp = 1,
	TA_Mirror = 2,
	TA_MAX = 3
};

// Object: Enum Engine.TextureFilter
enum class ETextureFilter : uint8_t
{
	TF_Nearest = 0,
	TF_Bilinear = 1,
	TF_Trilinear = 2,
	TF_Default = 3,
	TF_MAX = 4
};

// Object: Enum Engine.ETextureMobileCompressionSwizzle
enum class ETextureMobileCompressionSwizzle : uint8_t
{
	TMCS_Auto = 0,
	TMCS_bgra = 1,
	TMCS_bgr1 = 2,
	TMCS_0g0b = 3,
	TMCS_ggg1 = 4,
	TMCS_MAX = 5
};

// Object: Enum Engine.TexturePreset
enum class ETexturePreset : uint8_t
{
	TEXTUREPRESET_Custom = 0,
	TEXTUREPRESET_Project01 = 1,
	TEXTUREPRESET_Project02 = 2,
	TEXTUREPRESET_Project03 = 3,
	TEXTUREPRESET_Project04 = 4,
	TEXTUREPRESET_Project05 = 5,
	TEXTUREPRESET_Project06 = 6,
	TEXTUREPRESET_Project07 = 7,
	TEXTUREPRESET_Project08 = 8,
	TEXTUREPRESET_Project09 = 9,
	TEXTUREPRESET_Project10 = 10,
	TEXTUREPRESET_Project11 = 11,
	TEXTUREPRESET_Project12 = 12,
	TEXTUREPRESET_Project13 = 13,
	TEXTUREPRESET_Project14 = 14,
	TEXTUREPRESET_Project15 = 15,
	TEXTUREPRESET_Project16 = 16,
	TEXTUREPRESET_Project17 = 17,
	TEXTUREPRESET_Project18 = 18,
	TEXTUREPRESET_Project19 = 19,
	TEXTUREPRESET_Project20 = 20,
	TEXTUREPRESET_Project21 = 21,
	TEXTUREPRESET_Project22 = 22,
	TEXTUREPRESET_Project23 = 23,
	TEXTUREPRESET_Project24 = 24,
	TEXTUREPRESET_Project25 = 25,
	TEXTUREPRESET_Project26 = 26,
	TEXTUREPRESET_Project27 = 27,
	TEXTUREPRESET_Project28 = 28,
	TEXTUREPRESET_Project29 = 29,
	TEXTUREPRESET_Project30 = 30,
	TEXTUREPRESET_Project31 = 31,
	TEXTUREPRESET_Project32 = 32,
	TEXTUREPRESET_MAX = 33
};

// Object: Enum Engine.ETextureMobileCompressionQuality
enum class ETextureMobileCompressionQuality : uint8_t
{
	TMCQ_Default = 0,
	TMCQ_Lowest = 1,
	TMCQ_Low = 2,
	TMCQ_Medium = 3,
	TMCQ_High = 4,
	TMCQ_Highest = 5,
	TMCQ_MAX = 6
};

// Object: Enum Engine.ETextureMobileCompressionFormat
enum class ETextureMobileCompressionFormat : uint8_t
{
	TMCF_AUTO = 0,
	TMCF_ASTC4x4 = 1,
	TMCF_ASTC6x6 = 2,
	TMCF_ASTC8x8 = 3,
	TMCF_ASTC10x10 = 4,
	TMCF_ASTC12x12 = 5,
	TMCF_MAX = 6
};

// Object: Enum Engine.TextureCompressionSettings
enum class ETextureCompressionSettings : uint8_t
{
	TC_Default = 0,
	TC_Normalmap = 1,
	TC_Masks = 2,
	TC_Grayscale = 3,
	TC_Displacementmap = 4,
	TC_VectorDisplacementmap = 5,
	TC_HDR = 6,
	TC_EditorIcon = 7,
	TC_Alpha = 8,
	TC_DistanceFieldFont = 9,
	TC_HDR_Compressed = 10,
	TC_BC7 = 11,
	TC_HalfFloat = 12,
	TC_ReflectionCapture = 13,
	TC_MAX = 14
};

// Object: Enum Engine.ETextureDownscaleOptions
enum class ETextureDownscaleOptions : uint8_t
{
	Default = 0,
	Unfiltered = 1,
	SimpleAverage = 2,
	Sharpen0 = 3,
	Sharpen1 = 4,
	Sharpen2 = 5,
	Sharpen3 = 6,
	Sharpen4 = 7,
	Sharpen5 = 8,
	Sharpen6 = 9,
	Sharpen7 = 10,
	Sharpen8 = 11,
	Sharpen9 = 12,
	Sharpen10 = 13,
	ETextureDownscaleOptions_MAX = 14
};

// Object: Enum Engine.ETextureMipLoadOptions
enum class ETextureMipLoadOptions : uint8_t
{
	Default = 0,
	AllMips = 1,
	OnlyFirstMip = 2,
	ETextureMipLoadOptions_MAX = 3
};

// Object: Enum Engine.ETextureSamplerFilter
enum class ETextureSamplerFilter : uint8_t
{
	Point = 0,
	Bilinear = 1,
	Trilinear = 2,
	AnisotropicPoint = 3,
	AnisotropicLinear = 4,
	ETextureSamplerFilter_MAX = 5
};

// Object: Enum Engine.ETexturePowerOfTwoSetting
enum class ETexturePowerOfTwoSetting : uint8_t
{
	None = 0,
	PadToPowerOfTwo = 1,
	PadToSquarePowerOfTwo = 2,
	ETexturePowerOfTwoSetting_MAX = 3
};

// Object: Enum Engine.TextureMipGenSettings
enum class ETextureMipGenSettings : uint8_t
{
	TMGS_FromTextureGroup = 0,
	TMGS_SimpleAverage = 1,
	TMGS_Sharpen0 = 2,
	TMGS_Sharpen1 = 3,
	TMGS_Sharpen2 = 4,
	TMGS_Sharpen3 = 5,
	TMGS_Sharpen4 = 6,
	TMGS_Sharpen5 = 7,
	TMGS_Sharpen6 = 8,
	TMGS_Sharpen7 = 9,
	TMGS_Sharpen8 = 10,
	TMGS_Sharpen9 = 11,
	TMGS_Sharpen10 = 12,
	TMGS_NoMipmaps = 13,
	TMGS_LeaveExistingMips = 14,
	TMGS_Blur1 = 15,
	TMGS_Blur2 = 16,
	TMGS_Blur3 = 17,
	TMGS_Blur4 = 18,
	TMGS_Blur5 = 19,
	TMGS_Unfiltered = 20,
	TMGS_MAX = 21
};

// Object: Enum Engine.TextureGroup
enum class ETextureGroup : uint8_t
{
	TEXTUREGROUP_World = 0,
	TEXTUREGROUP_WorldNormalMap = 1,
	TEXTUREGROUP_WorldSpecular = 2,
	TEXTUREGROUP_Character = 3,
	TEXTUREGROUP_CharacterNormalMap = 4,
	TEXTUREGROUP_CharacterSpecular = 5,
	TEXTUREGROUP_Weapon = 6,
	TEXTUREGROUP_WeaponNormalMap = 7,
	TEXTUREGROUP_WeaponSpecular = 8,
	TEXTUREGROUP_Vehicle = 9,
	TEXTUREGROUP_VehicleNormalMap = 10,
	TEXTUREGROUP_VehicleSpecular = 11,
	TEXTUREGROUP_Cinematic = 12,
	TEXTUREGROUP_Effects = 13,
	TEXTUREGROUP_EffectsNotFiltered = 14,
	TEXTUREGROUP_Skybox = 15,
	TEXTUREGROUP_UI = 16,
	TEXTUREGROUP_Lightmap = 17,
	TEXTUREGROUP_RenderTarget = 18,
	TEXTUREGROUP_MobileFlattened = 19,
	TEXTUREGROUP_ProcBuilding_Face = 20,
	TEXTUREGROUP_ProcBuilding_LightMap = 21,
	TEXTUREGROUP_Shadowmap = 22,
	TEXTUREGROUP_ColorLookupTable = 23,
	TEXTUREGROUP_Terrain_Heightmap = 24,
	TEXTUREGROUP_Terrain_Weightmap = 25,
	TEXTUREGROUP_Bokeh = 26,
	TEXTUREGROUP_IESLightProfile = 27,
	TEXTUREGROUP_Pixels2D = 28,
	TEXTUREGROUP_HierarchicalLOD = 29,
	TEXTUREGROUP_Impostor = 30,
	TEXTUREGROUP_ImpostorNormalDepth = 31,
	TEXTUREGROUP_8BitData = 32,
	TEXTUREGROUP_16BitData = 33,
	TEXTUREGROUP_Project01 = 34,
	TEXTUREGROUP_Project02 = 35,
	TEXTUREGROUP_Project03 = 36,
	TEXTUREGROUP_Project04 = 37,
	TEXTUREGROUP_Project05 = 38,
	TEXTUREGROUP_Project06 = 39,
	TEXTUREGROUP_Project07 = 40,
	TEXTUREGROUP_Project08 = 41,
	TEXTUREGROUP_Project09 = 42,
	TEXTUREGROUP_Project10 = 43,
	TEXTUREGROUP_Project11 = 44,
	TEXTUREGROUP_Project12 = 45,
	TEXTUREGROUP_Project13 = 46,
	TEXTUREGROUP_Project14 = 47,
	TEXTUREGROUP_Project15 = 48,
	TEXTUREGROUP_MAX = 49
};

// Object: Enum Engine.ETextureRenderTargetFormat
enum class ETextureRenderTargetFormat : uint8_t
{
	RTF_R8 = 0,
	RTF_RG8 = 1,
	RTF_RGBA8 = 2,
	RTF_RGBA8_SRGB = 3,
	RTF_R16f = 4,
	RTF_RG16f = 5,
	RTF_RGBA16f = 6,
	RTF_R32f = 7,
	RTF_RG32f = 8,
	RTF_RGBA32f = 9,
	RTF_RGB10A2 = 10,
	RTF_MAX = 11
};

// Object: Enum Engine.ETimecodeProviderSynchronizationState
enum class ETimecodeProviderSynchronizationState : uint8_t
{
	Closed = 0,
	Error = 1,
	Synchronized = 2,
	Synchronizing = 3,
	ETimecodeProviderSynchronizationState_MAX = 4
};

// Object: Enum Engine.ETimelineDirection
enum class ETimelineDirection : uint8_t
{
	Forward = 0,
	Backward = 1,
	ETimelineDirection_MAX = 2
};

// Object: Enum Engine.ETimelineLengthMode
enum class ETimelineLengthMode : uint8_t
{
	TL_TimelineLength = 0,
	TL_LastKeyFrame = 1,
	TL_MAX = 2
};

// Object: Enum Engine.ETimeStretchCurveMapping
enum class ETimeStretchCurveMapping : uint8_t
{
	T_Original = 0,
	T_TargetMin = 1,
	T_TargetMax = 2,
	MAX = 3
};

// Object: Enum Engine.ETwitterIntegrationDelegate
enum class ETwitterIntegrationDelegate : uint8_t
{
	TID_AuthorizeComplete = 0,
	TID_TweetUIComplete = 1,
	TID_RequestComplete = 2,
	TID_MAX = 3
};

// Object: Enum Engine.ETwitterRequestMethod
enum class ETwitterRequestMethod : uint8_t
{
	TRM_Get = 0,
	TRM_Post = 1,
	TRM_Delete = 2,
	TRM_MAX = 3
};

// Object: Enum Engine.EUserDefinedStructureStatus
enum class EUserDefinedStructureStatus : uint8_t
{
	UDSS_UpToDate = 0,
	UDSS_Dirty = 1,
	UDSS_Error = 2,
	UDSS_Duplicate = 3,
	UDSS_MAX = 4
};

// Object: Enum Engine.EUIScalingRule
enum class EUIScalingRule : uint8_t
{
	ShortestSide = 0,
	LongestSide = 1,
	Horizontal = 2,
	Vertical = 3,
	ScaleToFit = 4,
	Custom = 5,
	EUIScalingRule_MAX = 6
};

// Object: Enum Engine.ERenderFocusRule
enum class ERenderFocusRule : uint8_t
{
	Always = 0,
	NonPointer = 1,
	NavigationOnly = 2,
	Never = 3,
	ERenderFocusRule_MAX = 4
};

// Object: Enum Engine.EVectorFieldConstructionOp
enum class EVectorFieldConstructionOp : uint8_t
{
	VFCO_Extrude = 0,
	VFCO_Revolve = 1,
	VFCO_MAX = 2
};

// Object: Enum Engine.EWeatherState
enum class EWeatherState : uint8_t
{
	SunnyState = 0,
	RainyState = 1,
	EWeatherState_MAX = 2
};

// Object: Enum Engine.EWindSourceType
enum class EWindSourceType : uint8_t
{
	Directional = 0,
	Point = 1,
	EWindSourceType_MAX = 2
};

// Object: Enum Engine.EPSCPoolMethod
enum class EPSCPoolMethod : uint8_t
{
	None = 0,
	AutoRelease = 1,
	ManualRelease = 2,
	ManualRelease_OnComplete = 3,
	FreeInPool = 4,
	EPSCPoolMethod_MAX = 5
};

// Object: Enum Engine.EVolumeLightingMethod
enum class EVolumeLightingMethod : uint8_t
{
	VLM_VolumetricLightmap = 0,
	VLM_SparseVolumeLightingSamples = 1,
	VLM_MAX = 2
};

// Object: Enum Engine.EVisibilityAggressiveness
enum class EVisibilityAggressiveness : uint8_t
{
	VIS_LeastAggressive = 0,
	VIS_ModeratelyAggressive = 1,
	VIS_MostAggressive = 2,
	VIS_Max = 3
};

// Package: MoeAsyncLoadingScreen
// Enums: 3

// Object: Enum MoeAsyncLoadingScreen.ELoadingWidgetType
enum class ELoadingWidgetType : uint8_t
{
	LWT_Horizontal = 0,
	LWT_Vertical = 1,
	LWT_MAX = 2
};

// Object: Enum MoeAsyncLoadingScreen.ELoadingIconType
enum class ELoadingIconType : uint8_t
{
	LIT_Throbber = 0,
	LIT_CircularThrobber = 1,
	LIT_ImageSequence = 2,
	LIT_SpinningImages = 3,
	LIT_MAX = 4
};

// Object: Enum MoeAsyncLoadingScreen.EAsyncLoadingScreenLayout
enum class EAsyncLoadingScreenLayout : uint8_t
{
	ALSL_Classic = 0,
	ALSL_Center = 1,
	ALSL_Letterbox = 2,
	ALSL_Sidebar = 3,
	ALSL_DualSidebar = 4,
	ALSL_MAX = 5
};

// Package: MoeGameCore
// Enums: 410

// Object: Enum MoeGameCore.ECharacterType
enum class ECharacterType : uint8_t
{
	Character = 0,
	Monster = 1,
	NPC = 2,
	Vehicle = 3,
	ECharacterType_MAX = 4
};

// Object: Enum MoeGameCore.EMovementAuthorityRole
enum class EMovementAuthorityRole : uint8_t
{
	ROLE_Server = 0,
	ROLE_Client = 1,
	ROLE_MAX = 2
};

// Object: Enum MoeGameCore.ECameraType
enum class ECameraType : uint8_t
{
	TP_FREE = 0,
	TP_FOLLOW = 1,
	FP = 2,
	DEATH = 3,
	UGC_Editor = 4,
	JS_SNOWBOARD = 5,
	MCG_FOLLOW = 6,
	MAY_FOLLOW = 7,
	ECameraType_MAX = 8
};

// Object: Enum MoeGameCore.ECustomInputType
enum class ECustomInputType : uint8_t
{
	None = 0,
	Fixed_2D = 1,
	ECustomInputType_MAX = 2
};

// Object: Enum MoeGameCore.EPlayerDisconnectReason
enum class EPlayerDisconnectReason : uint8_t
{
	EPlayerDisconnectReason_DsPingTimeOut = 0,
	EPlayerDisconnectReason_CloseConnection = 1,
	EPlayerDisconnectReason_MAX = 2
};

// Object: Enum MoeGameCore.EMoeCameraFeatureType
enum class EMoeCameraFeatureType : uint8_t
{
	Default = 0,
	OGC = 1,
	UGC = 2,
	SP = 3,
	Chest = 20,
	ChestPVE = 21,
	EMoeCameraFeatureType_MAX = 22
};

// Object: Enum MoeGameCore.EMoePlayerControllerType
enum class EMoePlayerControllerType : uint8_t
{
	InGame = 0,
	Lobby = 1,
	DDP = 2,
	UGC = 3,
	Home = 4,
	Farm = 5,
	Cook = 6,
	House = 7,
	SPGame = 8,
	HouseEdit = 9,
	FarmCrazy = 10,
	UnKnow = 11,
	EMoePlayerControllerType_MAX = 12
};

// Object: Enum MoeGameCore.EMoeAComponentSpawnCondition
enum class EMoeAComponentSpawnCondition : uint8_t
{
	EMOEACSC_Multi = 0,
	EMOEACSC_OnlyOnAllClients = 1,
	EMOEACSC_OnlyOnServer = 2,
	EMOEACSC_OnlyOnAutonomousProxy = 3,
	EMOEACSC_OnlyOnSimulatedProxy = 4,
	EMOEACSC_MultiNotReplicate = 5,
	EMOEACSC_MAX = 6
};

// Object: Enum MoeGameCore.EChangeClipState
enum class EChangeClipState : uint8_t
{
	BeginChangeClip = 0,
	BeginHoldMag = 1,
	BeginHideMag = 2,
	BeginShowMag = 3,
	BeginHoldMagEnd = 4,
	EndChangeClip = 5,
	FinishOnceClip = 6,
	EChangeClipState_MAX = 7
};

// Object: Enum MoeGameCore.ESGWeaponType
enum class ESGWeaponType : uint8_t
{
	Unknown = 0,
	EWP_M4 = 1,
	EWP_BaseBallBat = 2,
	EWP_RPG = 3,
	EWP_M249 = 4,
	EWP_Knife = 5,
	EWP_M1897 = 6,
	EWP_AWM = 7,
	EWP_Taser = 8,
	EWP_M249Grenade = 9,
	EWP_Basketball = 10,
	EWP_M1911 = 11,
	EWP_UZI = 12,
	EWP_Grenade = 13,
	EWP_Landmine = 14,
	EWP_BPMBombC4 = 15,
	EWP_DrivingTurret = 16,
	EWP_MoneyGun = 17,
	EWP_Nunchaku = 18,
	EWP_AA12 = 19,
	EWP_MGL = 20,
	EWP_SP_Unarmed = 21,
	EWP_UGCWeapon = 31,
	EWP_UGCSkill = 32,
	EWP_PUNCH = 80,
	EWP_Prop_Begin = 128,
	EWP_Prop_ScreamingChicken = 129,
	EWP_Prop_Boomerang = 130,
	EWP_Prop_Tomato = 131,
	EWP_Prop_Bomb = 132,
	EWP_Prop_IceBomb = 133,
	EWP_Prop_FireWork = 134,
	EWP_Prop_Landmine = 135,
	EWP_Prop_RotateHammer = 136,
	EWP_Prop_VacuumCleaner = 137,
	EWP_Prop_Laser = 138,
	EWP_Prop_End = 139,
	EWP_SP_Weapon_HeavySword = 160,
	EWP_SP_Weapon_CrossBow = 161,
	EWP_SP_Weapon_HandGun = 162,
	EWP_SP_Weapon_ShotGun = 163,
	EWP_SP_Weapon_Rifle = 164,
	EWP_SP_Weapon_Tool_Torch = 165,
	EWP_SP_Weapon_Work_Pic = 166,
	EWP_SP_Weapon_Work_Ax = 167,
	EWP_SP_Weapon_PokemonGun = 168,
	EWP_SP_Weapon_Staff_Base = 169,
	EWP_SP_Weapon_Staff_Attack = 170,
	EWP_SP_Weapon_Staff_Treat = 171,
	EWP_SP_Weapon_BaseBallBat = 172,
	EWP_SP_Weapon_Sword = 173,
	EWP_SP_Weapon_RPG = 174,
	ESGWeaponType_MAX = 175
};

// Object: Enum MoeGameCore.ECharActionState
enum class ECharActionState : uint8_t
{
	None = 0,
	GettingUp = 1,
	Stuning = 2,
	Rebirth = 3,
	Dive = 4,
	TryGrab = 5,
	EmoteAction = 6,
	Bounce = 7,
	Bubble = 8,
	Accel = 9,
	InteractAction = 10,
	EnterGame = 11,
	Lock = 12,
	Gliding = 13,
	PoleVault = 14,
	Conveyor = 15,
	BodyCollision = 16,
	Frozen = 17,
	WeightLoss = 18,
	Booster = 19,
	PropShoot = 20,
	FPS_Electrocute = 21,
	FPS_Stuning = 22,
	Palsy = 23,
	InWind = 24,
	OnFireworks = 25,
	AwaitingDeath = 26,
	Dialogue = 27,
	DeathWithAnim = 28,
	RotateHammer = 29,
	Dizziness = 30,
	OutChocolate = 31,
	Push = 32,
	SpringBoard = 33,
	Trampoline = 34,
	Grab = 35,
	GrabCharacter = 36,
	GrabBasketBall = 37,
	VirtualAction = 38,
	ThrowProp = 39,
	IdleShow = 40,
	ProjectIdleShow = 41,
	WaveChicken = 42,
	EasterEggShow = 43,
	Br_Prop = 44,
	FishingProp = 45,
	FollowAction = 46,
	BubbleGun = 47,
	InMonkiBubble = 48,
	VacuumCleaner = 49,
	Br_Pick = 50,
	UltramanRaySkill = 51,
	PropUse = 52,
	ExclusiveVehicleAction = 53,
	SkippingRope_Waiting = 54,
	SkippingRope_Shake = 55,
	SkippingRope_Jump = 56,
	Effect = 57,
	PerformSuitMorphAnim = 58,
	NR3E_Idle = 59,
	InteractiveActor = 60,
	Precast = 61,
	DDP_Begin = 62,
	LightPunch = 63,
	MeleeWeapon = 64,
	RangeWeapon = 65,
	GunHolding = 66,
	GunFiring = 67,
	PA_AttackByHead = 68,
	PA_Dead = 69,
	PA_JumpAttack = 70,
	PA_FallDown = 71,
	PA_Ragdoll = 72,
	PA_Freeze = 73,
	PA_Electrocute = 74,
	PA_AttackByLeg = 75,
	DDP_ContiniousAttackWeapon = 76,
	DDP_BlownUp = 77,
	SlideWay_Ground = 78,
	SlideWay_Jump = 79,
	SlideWay_Dive = 80,
	SlideWay_GettingUp = 81,
	UGC_Begin = 82,
	UGCSkillPrecast = 83,
	UGCSkillChanneling = 84,
	UGCSkillAttack = 85,
	UGCSkillBackSwing = 86,
	UGC_End = 87,
	PA_FastAttackByLeg = 88,
	PA_FastFist = 89,
	PA_HeavyFist = 90,
	PA_RagdollWaittingInput = 91,
	PA_BeGrabbedJump = 92,
	Farm_Begin = 93,
	Farm_Fish = 94,
	Farm_ShowOff = 95,
	Farm_Action = 96,
	Farm_Bounce = 97,
	Farm_Shapeshift = 98,
	Farm_BroomMagic = 99,
	Farm_CrazyBounce = 100,
	Farm_CrazyFish = 101,
	Farm_CrazyShowOff = 102,
	Farm_CrazyRodSurfing = 103,
	Farm_End = 104,
	Mst_InitialState = 105,
	Mst_Searching = 106,
	Mst_PlayerFoundReaction = 107,
	Mst_Chasing = 108,
	Mst_PrepareAttack = 109,
	Mst_Attacking = 110,
	Mst_Stun = 111,
	Mst_Idle = 112,
	Mst_EnterGame = 113,
	Mst_Escape = 114,
	Mst_Wander = 115,
	Mst_Jump = 116,
	Mst_JumpDown = 117,
	XW_InteractLock = 118,
	XW_HandHeldUse = 119,
	MCG_Begin = 120,
	MCG_ClimbWindow = 121,
	MCG_ClimbWoodenVan = 122,
	MCG_DropWoodenVan = 123,
	MCG_OpenDoor = 124,
	MCG_ReadingStarChart = 125,
	MCG_Struggle = 126,
	MCG_HealTeammates = 127,
	MCG_RescueTeammates = 128,
	MCG_RescuedSuccess = 129,
	MCG_RescueFromAltar = 130,
	MCG_DecipherGate = 131,
	MCG_BossCatch = 132,
	MCG_BossRelease = 133,
	MCG_BossExile = 134,
	MCG_BossStub = 135,
	MCG_BossBreakBoard = 136,
	MCG_BossFishingRod = 137,
	MCG_BossFreeze = 138,
	MCG_BossDestroyStarChart = 139,
	MCG_CommonUpper = 140,
	MCG_CommonUpper2 = 141,
	MCG_CommonSkillLoop = 142,
	MCG_PlayerDead = 143,
	MCG_PlayerStub = 144,
	MCG_PlayerPutBoard = 145,
	MCG_Bounce = 146,
	MCG_ReleaseBack = 147,
	MCG_StarRoad = 148,
	MCG_Addition1 = 149,
	MCG_Addition2 = 150,
	MCG_PlayerMiaExecution = 151,
	MCG_MiaExecution = 152,
	MCG_BubbleImprison = 153,
	MCG_CommonInteract = 154,
	MCG_ReadingMap = 155,
	MCG_HidingOut = 156,
	MCG_Max = 157,
	DDB_Begin = 158,
	DDB_DuckWave = 159,
	DDB_CharStateBodyCollision = 160,
	DDB_End = 161,
	Arena_Begin = 162,
	Arena_KnockFly = 163,
	Arena_KnockDown = 164,
	Arena_BeakBack = 165,
	Arena_PlaySkill = 166,
	Arena_End = 167,
	CBP_HoldRedEnvelope = 168,
	Bnb_Arrested = 169,
	InteractionSequence = 170,
	Action_Universal = 171,
	Action_Universal = 172,
	Action_Universal = 173,
	Action_Universal = 174,
	Action_Universal = 175,
	Action_Universal = 176,
	Action_Universal = 177,
	Action_Universal = 178,
	Action_Universal = 179,
	Action_Universal = 180,
	Action_Universal = 181,
	Action_Universal = 182,
	Action_Universal = 183,
	HandWearAction = 184,
	Skateboard = 185,
	ShuttleFly = 186,
	Treasure_Search = 187,
	ActionCommon = 188,
	Max = 189,
	Unknown = 190
};

// Object: Enum MoeGameCore.ECharMotionState
enum class ECharMotionState : uint8_t
{
	Default = 0,
	Slip = 1,
	Swim = 2,
	SlipPipe = 3,
	BaseJump = 4,
	InChocolate = 5,
	InIce = 6,
	InWind = 7,
	BeGrabed = 8,
	Sprinted = 9,
	Teleport = 10,
	InBallCar = 11,
	Banana = 12,
	SlideWay = 13,
	JumpSlideWay = 14,
	TargetedMove = 15,
	Follow = 16,
	GachaBall = 17,
	InMonkiBubbles = 18,
	Tricycle = 19,
	UGCVehicle = 20,
	Swing = 21,
	Snake = 22,
	DriveExclusiveVehicle = 23,
	RideExclusiveVehicle = 24,
	SkippingRope = 25,
	Effect = 26,
	Backtrack = 27,
	Fly = 28,
	DriverTank = 29,
	FlappyBird = 30,
	SlideRail = 31,
	Lightning = 32,
	GeneralTargetedMove = 33,
	Slide = 34,
	DDP_Begin = 64,
	PA_Run = 65,
	PA_BeGrabbed = 66,
	PA_GrabCharacterMove = 67,
	PA_GrabCharacterRun = 68,
	PA_GrabCharacterJump = 69,
	MCG_Begin = 168,
	MCG_Normal = 169,
	MCG_Injuried = 170,
	MCG_Dying = 171,
	MCG_Caught = 172,
	MCG_InChair = 173,
	MCG_Dead = 174,
	MCG_BossCatching = 175,
	MCG_Interaction = 176,
	MCG_InGourd = 177,
	MCG_OnBack = 178,
	MCG_CarryBack = 179,
	MCG_Vehicle = 180,
	MCG_Addition1 = 181,
	MCG_Addition2 = 182,
	MCG_Max = 183,
	Arena_CarDriver = 210,
	Arena_CarRider = 211,
	Arena_Chaos = 212,
	Arena_Enchant = 213,
	Arena_Harvester = 214,
	Oversea_GuidePerform = 215,
	DDB_Begin = 224,
	DDB_Backtrack = 225,
	DDB_End = 226,
	Arena_Begin = 230,
	Arena_SkillMove = 231,
	Arena_FollowMove = 232,
	Arena_End = 233,
	XW_HandHeldEquip = 234,
	Motion_Universal = 235,
	Motion_Universal = 236,
	Motion_Universal = 237,
	Motion_Universal = 238,
	Motion_Universal = 239,
	Motion_Universal = 240,
	Motion_Universal = 241,
	Motion_Universal = 242,
	Motion_Universal = 243,
	Motion_Universal = 244,
	Motion_Universal = 245,
	Motion_Universal = 246,
	Motion_Universal = 247,
	Max = 248
};

// Object: Enum MoeGameCore.EStateIdOffsetType
enum class EStateIdOffsetType : uint8_t
{
	Default = 0,
	Arena = 1,
	OGC = 2,
	Mayday = 3,
	SP = 4,
	Pet = 5,
	CHT = 6,
	Max = 7
};

// Object: Enum MoeGameCore.EUGCAIInputIgnoreType
enum class EUGCAIInputIgnoreType : uint8_t
{
	None = 0,
	Jump = 1,
	Dive = 2,
	Grab = 4,
	ItemUse = 8,
	Move = 16,
	IgnoreAll = 31,
	EUGCAIInputIgnoreType_MAX = 32
};

// Object: Enum MoeGameCore.EShootDamageType
enum class EShootDamageType : uint8_t
{
	Normal = 0,
	Burning = 1,
	Explore = 2,
	Gamble = 3,
	Critical = 4,
	PoisonCircle = 5,
	Electric = 6,
	CounterAttack = 7,
	Poision = 8,
	Cold = 9,
	OMDDeputySkill = 10,
	OMDNormalSkill = 11,
	UGCSkill = 12,
	Laser = 13,
	PropNormal = 128,
	Server = 188,
	EShootDamageType_MAX = 189
};

// Object: Enum MoeGameCore.EMoveState
enum class EMoveState : uint8_t
{
	EMS_Idle = 0,
	EMS_Sprint = 1,
	EMS_Walk = 2,
	EMS_Creep = 3,
	EMS_CreepBack = 4,
	EMS_MAX = 5
};

// Object: Enum MoeGameCore.EEventDebugModule
enum class EEventDebugModule : uint8_t
{
	None = 0,
	UGCSignal = 1,
	EEventDebugModule_MAX = 2
};

// Object: Enum MoeGameCore.EHitSizeLevel
enum class EHitSizeLevel : uint8_t
{
	Light = 0,
	Medium = 1,
	Hard = 2,
	None = 3,
	EHitSizeLevel_MAX = 4
};

// Object: Enum MoeGameCore.EWayPointStartPointControlType
enum class EWayPointStartPointControlType : uint8_t
{
	Independent = 0,
	Unit = 1,
	Attach = 2,
	EWayPointStartPointControlType_MAX = 3
};

// Object: Enum MoeGameCore.EWayPointMoveType
enum class EWayPointMoveType : uint8_t
{
	Linear = 0,
	Curve = 1,
	Mix = 2,
	EWayPointMoveType_MAX = 3
};

// Object: Enum MoeGameCore.EBindRotation
enum class EBindRotation : uint8_t
{
	None = 0,
	BindX = 1,
	BindY = 2,
	BindZ = 3,
	EBindRotation_MAX = 4
};

// Object: Enum MoeGameCore.EVelocityType
enum class EVelocityType : uint8_t
{
	uniform = 0,
	accelerated = 1,
	decelerated = 2,
	smooth = 3,
	swing = 4,
	shake = 5,
	custom = 6,
	EVelocityType_MAX = 7
};

// Object: Enum MoeGameCore.EMoveType
enum class EMoveType : uint8_t
{
	Location = 0,
	Rotation = 1,
	Scale = 2,
	Spline = 3,
	WayPoint = 4,
	DeltaScale = 5,
	EMoveType_MAX = 6
};

// Object: Enum MoeGameCore.EMoeEventTriggerType
enum class EMoeEventTriggerType : uint8_t
{
	Global = 0,
	Group = 1,
	Self = 2,
	Other = 3,
	EMoeEventTriggerType_MAX = 4
};

// Object: Enum MoeGameCore.EEffectAddTarget
enum class EEffectAddTarget : uint8_t
{
	Other = 0,
	Self = 1,
	EEffectAddTarget_MAX = 2
};

// Object: Enum MoeGameCore.EEffectNetRole
enum class EEffectNetRole : uint8_t
{
	AutonomousAndAuthority = 0,
	Autonomous = 1,
	Authority = 2,
	All = 3,
	AutonomousAndSimulate = 4,
	EEffectNetRole_MAX = 5
};

// Object: Enum MoeGameCore.EMoePropState
enum class EMoePropState : uint8_t
{
	NotPick = 0,
	OnHand = 1,
	PrepareUse = 2,
	AfterUse = 3,
	AfterHit = 4,
	InOperation = 5,
	EMoePropState_MAX = 6
};

// Object: Enum MoeGameCore.ECharAffectedActionType
enum class ECharAffectedActionType : uint8_t
{
	PropHit = 0,
	EndGrab = 1,
	Dive = 2,
	LevelSpecial = 3,
	GameplayAbility = 4,
	ECharAffectedActionType_MAX = 5
};

// Object: Enum MoeGameCore.EMoeCaptureState
enum class EMoeCaptureState : uint8_t
{
	FollowCharacter = 0,
	FollowProActor = 1,
	EMoeCaptureState_MAX = 2
};

// Object: Enum MoeGameCore.ECharSmoothMoveResult
enum class ECharSmoothMoveResult : uint8_t
{
	Success = 0,
	Fail = 1,
	Abort = 2,
	Interrupted = 3,
	ECharSmoothMoveResult_MAX = 4
};

// Object: Enum MoeGameCore.EMoePropTargetSlot
enum class EMoePropTargetSlot : uint8_t
{
	Default = 0,
	EasterEggSlot = 1,
	Cache = 2,
	Bag = 3,
	Extra = 4,
	DDB = 5,
	Br = 6,
	Bnb = 7,
	Ability = 8,
	C4Bomb = 9,
	ColorMatch = 10,
	Tank = 11,
	NR3E = 12,
	BasketBall = 13,
	GameplayAbility = 14,
	MAYDAY_CHEST = 15,
	MAYDAY_LeftHand = 16,
	MAYDAY_HAND = 17,
	CHEST_Deafalut = 31,
	CHEST_Slot1 = 32,
	CHEST_Slot2 = 33,
	CHEST_Slot3 = 34,
	CHEST_Slot4 = 35,
	CHEST_Slot5 = 36,
	CHEST_Slot6 = 37,
	CHEST_Common1 = 38,
	CHEST_Common2 = 39,
	CHEST_ActionID1 = 40,
	CHEST_ActionID2 = 41,
	CHEST_ActionID3 = 42,
	CHEST_ActionID4 = 43,
	CHEST_ActionID5 = 44,
	CHEST_ActionID6 = 45,
	CHEST_ActionID7 = 46,
	CHEST_ActionID8 = 47,
	CHEST_ActionID9 = 48,
	Invalid = 100,
	EMoePropTargetSlot_MAX = 101
};

// Object: Enum MoeGameCore.EMoePropUseStateType
enum class EMoePropUseStateType : uint8_t
{
	None = 0,
	Prepare = 1,
	Start = 2,
	Operation = 3,
	End = 4,
	EMoePropUseStateType_MAX = 5
};

// Object: Enum MoeGameCore.EMoeAnimModule_LocomotionMoveState
enum class EMoeAnimModule_LocomotionMoveState : uint8_t
{
	Moe_OnGroundMove = 0,
	Moe_OnBaseJump = 1,
	Moe_OnFall = 2,
	Moe_Land = 3,
	Moe_OnBeGrab = 4,
	Moe_TargetedMove = 5,
	Moe_InBubbles = 6,
	Moe_OnVehicle = 7,
	Moe_Climb = 8,
	Moe_SwimInWater = 9,
	Moe_OnExclusiveVehicle = 10,
	Moe_OnArenaCar = 11,
	Moe_Fly = 12,
	Moe_Tank = 13,
	Moe_Shuttle = 14,
	Moe_Slide = 15,
	Moe_SlidePipe = 16,
	Oversea_GuidePerform = 17,
	Moe_RodSurfing = 18,
	EMoeAnimModule_MAX = 19
};

// Object: Enum MoeGameCore.EMoeAnimModule_LocoGroundMoveState
enum class EMoeAnimModule_LocoGroundMoveState : uint8_t
{
	Moe_OnGround = 0,
	Moe_OnConveyor = 1,
	Moe_OnChocolate = 2,
	Moe_OnIce = 3,
	Moe_Sprinted = 4,
	Moe_Banana = 5,
	Moe_SlideWay = 6,
	Arena_SkillMove = 7,
	Moe_SlideRail = 8,
	Moe_Tank = 9,
	EMoeAnimModule_MAX = 10
};

// Object: Enum MoeGameCore.EMoeAnimModule_LocoJumpState
enum class EMoeAnimModule_LocoJumpState : uint8_t
{
	Moe_StartJump = 0,
	Moe_JumpLoop = 1,
	Moe_JumpStartSlideWay = 2,
	Moe_JumpLoopSlideWay = 3,
	Moe_JumEnd = 4,
	Moe_MAX = 5
};

// Object: Enum MoeGameCore.EMoeAnimModule_AnimBodyActionState
enum class EMoeAnimModule_AnimBodyActionState : uint8_t
{
	Moe_InStun = 0,
	Moe_GettingUp = 1,
	Moe_Diving = 2,
	Moe_IdleShow = 3,
	Moe_ProjectIdleShow = 4,
	Moe_EasterEggShow = 5,
	Moe_Frozen = 6,
	Moe_Glide = 7,
	Moe_PoleVault = 8,
	Moe_OnFireworks = 9,
	Moe_Diving_SlideWay = 10,
	Moe_Diving_GettingUp_SlideWay = 11,
	Moe_WeightLoss = 12,
	Moe_BounceFly = 13,
	Moe_Booster = 14,
	Moe_InWind = 15,
	Moe_Palsy = 16,
	Moe_AwaitingDeath = 17,
	Moe_Acceleration = 18,
	Moe_RotateHammer = 19,
	Moe_Dizziness = 20,
	Moe_VacuumCleaner = 21,
	Moe_Interactive = 22,
	Moe_UltramanSkill = 23,
	Moe_PropUseSequence = 24,
	Moe_PropUseBlendSpace = 25,
	Moe_BnbArrested = 26,
	Moe_FarmBounceFly = 27,
	Moe_PerformSuitMorphAnim = 28,
	Moe_GhostStand = 29,
	SP_SkillPlaying = 30,
	SP_Constructing = 31,
	SP_Crafting = 32,
	SP_Clim = 33,
	SP_Swim = 34,
	SP_Ride = 35,
	SP_Fly = 36,
	SP_FeedPal = 37,
	SP_Stroke = 38,
	SP_Stiff = 39,
	SP_Stun = 40,
	SP_BeckonPal = 41,
	SP_Glide = 42,
	SP_Tumbling = 43,
	SP_Tumbling_Sliding = 44,
	SP_Hit_Light = 45,
	SP_Hit_Back = 46,
	SP_Hit_Fly = 47,
	SP_Frozen = 48,
	SP_Dizziness = 49,
	SP_FeedPalTwo = 50,
	SP_AffinityOne = 51,
	SP_AffinityTwo = 52,
	SP_AffinityThree = 53,
	SP_Pick = 54,
	SP_Relieve = 55,
	Moe_IdleShowMontage = 56,
	MM_Anim201 = 201,
	MM_Anim202 = 202,
	MM_Anim203 = 203,
	EMoeAnimModule_MAX = 204
};

// Object: Enum MoeGameCore.EMoeAnimModule_AnimUpperBodyActionState
enum class EMoeAnimModule_AnimUpperBodyActionState : uint8_t
{
	Moe_Default = 0,
	Moe_Grab = 1,
	Moe_Pull = 2,
	Moe_ThrowProp = 3,
	Moe_WaveChicken = 4,
	Moe_PropShoot = 5,
	Moe_TrainLoop = 6,
	Moe_FishingProp = 7,
	Moe_BubbleGunProp = 8,
	Moe_VacuumCleaner = 9,
	Moe_ChineseDragon = 10,
	Moe_ChineseDragonHead = 11,
	Moe_ChineseDragonIdleShow = 12,
	Moe_RotateHammer = 13,
	Moe_UseProp = 14,
	Moe_HoldRedEnvelope = 15,
	Moe_PropUseSequence = 16,
	Moe_PropUseBlendSpace = 17,
	Moe_TreasureHurt = 18,
	Moe_FarmFish = 19,
	Moe_FarmShowOff = 20,
	Moe_FarmAction = 21,
	Moe_GrabBasketBall = 22,
	Moe_MAX = 23
};

// Object: Enum MoeGameCore.EGrabThrowType
enum class EGrabThrowType : uint8_t
{
	End = 0,
	Throw = 1,
	Jump = 2,
	NoImpulse = 3,
	EGrabThrowType_MAX = 4
};

// Object: Enum MoeGameCore.EMoeIaState
enum class EMoeIaState : uint8_t
{
	None = 0,
	Entering = 1,
	Gaming = 2,
	Exiting = 3,
	Creating = 4,
	Destroy = 5,
	EMoeIaState_MAX = 6
};

// Object: Enum MoeGameCore.ECharStateMode
enum class ECharStateMode : uint8_t
{
	Default = 0,
	DDP = 1,
	Monster = 2,
	DDB = 3,
	MCG = 4,
	DND = 5,
	Arena = 6,
	UGCCombine = 7,
	MAY = 8,
	NR3E1 = 9,
	OGC = 10,
	SPGame = 11,
	SPGameMonster = 12,
	Farm = 13,
	Chest = 14,
	Pet = 15,
	ProjectT = 16,
	ECharStateMode_MAX = 17
};

// Object: Enum MoeGameCore.EMobileTouchType
enum class EMobileTouchType : uint8_t
{
	Begin = 0,
	FirstMove = 1,
	Move = 2,
	End = 3,
	EMobileTouchType_MAX = 4
};

// Object: Enum MoeGameCore.EMobileControlButtonType
enum class EMobileControlButtonType : uint8_t
{
	EControlButtonType_Jump = 0,
	EControlButtonType_Fire = 1,
	EControlButtonType_None = 2,
	EControlButtonType_MAX = 3
};

// Object: Enum MoeGameCore.EMoeCommunityFacilityOpType
enum class EMoeCommunityFacilityOpType : uint8_t
{
	None = 0,
	StartUse = 1,
	EndUse = 2,
	EMoeCommunityFacilityOpType_MAX = 3
};

// Object: Enum MoeGameCore.EMoeIaAniTargetType
enum class EMoeIaAniTargetType : uint8_t
{
	NoTarget = 0,
	Point = 1,
	RelativeTrans = 2,
	EMoeIaAniTargetType_MAX = 3
};

// Object: Enum MoeGameCore.ELearningAIAgentState
enum class ELearningAIAgentState : uint8_t
{
	Invalid = 0,
	Initialized = 1,
	Started = 2,
	ELearningAIAgentState_MAX = 3
};

// Object: Enum MoeGameCore.ELearningAIErrorCode
enum class ELearningAIErrorCode : uint8_t
{
	None = 0,
	StartFailed = 1,
	TimedOut = 2,
	Other = 100,
	ELearningAIErrorCode_MAX = 101
};

// Object: Enum MoeGameCore.EAIActionType
enum class EAIActionType : uint8_t
{
	AIAction_None = 0,
	AIAction_Jump = 1,
	AIAction_Dive = 2,
	AIAction_Wait = 3,
	AIAction_Move = 4,
	AIAction_Fire = 5,
	AIAction_MAX = 6
};

// Object: Enum MoeGameCore.EDirectionTestState
enum class EDirectionTestState : uint8_t
{
	Normal = 0,
	Failed = 1,
	Passed = 2,
	EDirectionTestState_MAX = 3
};

// Object: Enum MoeGameCore.EAIType
enum class EAIType : uint8_t
{
	Empty = 0,
	EQSAI = 1,
	BTAI = 2,
	LearningAI = 3,
	LuaProgramAI = 4,
	EAIType_MAX = 5
};

// Object: Enum MoeGameCore.EArmorMeshParamType
enum class EArmorMeshParamType : uint8_t
{
	ETypeFloat = 0,
	ETypeColor = 1,
	EArmorMeshParamType_MAX = 2
};

// Object: Enum MoeGameCore.EArmorEffectType
enum class EArmorEffectType : uint8_t
{
	Hit = 0,
	Destroy = 1,
	EArmorEffectType_MAX = 2
};

// Object: Enum MoeGameCore.EParticleParamType
enum class EParticleParamType : uint8_t
{
	ETypeInt = 0,
	ETypeFloat = 1,
	ETypeVector = 2,
	ETypeColor = 3,
	ETypeBool = 4,
	EParticleParamType_MAX = 5
};

// Object: Enum MoeGameCore.EOverseaGuideActionState
enum class EOverseaGuideActionState : uint8_t
{
	OverseaGuide_Idle = 0,
	OverseaGuide_ShiFaToShanYao = 1,
	OverseaGuide_SayHi = 2,
	OverseaGuide_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_FarmHoldBlendType
enum class EMoeAnimModule_FarmHoldBlendType : uint8_t
{
	None = 0,
	LeftHand = 1,
	TwoHand = 2,
	RightHand = 3,
	EMoeAnimModule_MAX = 4
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionFarmBounceFly
enum class EMoeAnimModule_ActionFarmBounceFly : uint8_t
{
	Moe_FarmBounceFlyStart = 0,
	Moe_FarmBounceFlyLoop = 1,
	Moe_FarmBounceFlyEnd = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionFarmAction
enum class EMoeAnimModule_ActionFarmAction : uint8_t
{
	Moe_FarmActionStart = 0,
	Moe_FarmActionLoop = 1,
	Moe_FarmActionEnd = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionFarmShowOff
enum class EMoeAnimModule_ActionFarmShowOff : uint8_t
{
	Moe_FarmShowOffStart = 0,
	Moe_FarmShowOffLoop = 1,
	Moe_FarmShowOffEnd = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionFarmFish
enum class EMoeAnimModule_ActionFarmFish : uint8_t
{
	Moe_FarmFishIdle = 0,
	Moe_FarmFishHold = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionPropUseState
enum class EMoeAnimModule_ActionPropUseState : uint8_t
{
	Moe_PropUsePrepare = 0,
	Moe_PropUseStart = 1,
	Moe_PropUseOperation = 2,
	Moe_PropUseEnd = 3,
	Moe_MAX = 4
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionUltramanSkill
enum class EMoeAnimModule_ActionUltramanSkill : uint8_t
{
	Moe_UltramanSkillStart = 0,
	Moe_UltramanSkillLoop = 1,
	Moe_UltramanSkillEnd = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_HoldeRedEnvelope
enum class EMoeAnimModule_HoldeRedEnvelope : uint8_t
{
	Moe_HoldStart = 0,
	Moe_HoldIdle = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_OnArenaCarMotionState
enum class EMoeAnimModule_OnArenaCarMotionState : uint8_t
{
	Moe_OnDriving = 0,
	Moe_OnFriendRiding = 1,
	Moe_OnEnemy1Riding = 2,
	Moe_OnEnemy2Riding = 3,
	Moe_OnHarvester = 4,
	Moe_MAX = 5
};

// Object: Enum MoeGameCore.EMoeAnimModule_OnFlyMotionState
enum class EMoeAnimModule_OnFlyMotionState : uint8_t
{
	Moe_Flying = 0,
	Moe_FlyDive = 1,
	Moe_FlySprint = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_OnExclusiveVehicleJumpState
enum class EMoeAnimModule_OnExclusiveVehicleJumpState : uint8_t
{
	Moe_VehicleStartJump = 0,
	Moe_VehicleLoopJump = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_OnExclusiveVehicleAnimState
enum class EMoeAnimModule_OnExclusiveVehicleAnimState : uint8_t
{
	Moe_OnDefault = 0,
	Moe_OnJump = 1,
	Moe_OnFall = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_OnExclusiveVehicleMotionState
enum class EMoeAnimModule_OnExclusiveVehicleMotionState : uint8_t
{
	Moe_OnDriving = 0,
	Moe_OnRiding = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_OnVehicleMotionState
enum class EMoeAnimModule_OnVehicleMotionState : uint8_t
{
	Moe_OnDriving = 0,
	Moe_OnRiding = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionInteractiveState
enum class EMoeAnimModule_ActionInteractiveState : uint8_t
{
	Moe_InteractiveEnter = 0,
	Moe_InteractiveIdle = 1,
	Moe_InteractiveExit = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_LocoInBubblesMoveState
enum class EMoeAnimModule_LocoInBubblesMoveState : uint8_t
{
	Moe_FrontInBubbles = 0,
	Moe_BackInBubbles = 1,
	Moe_InBubblesLoop = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_SkillImpactAnimState
enum class EMoeAnimModule_SkillImpactAnimState : uint8_t
{
	None = 0,
	Blast = 1,
	Backlash = 2,
	Stun = 3,
	Numb = 4,
	EMoeAnimModule_MAX = 5
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionUGCSkill
enum class EMoeAnimModule_ActionUGCSkill : uint8_t
{
	Moe_UGCSkillNone = 0,
	Moe_UGCSkillPrecast = 1,
	Moe_UGCSkillChanneling = 2,
	Moe_UGCSkillAttack = 3,
	Moe_MAX = 4
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionPropBubbleGun
enum class EMoeAnimModule_ActionPropBubbleGun : uint8_t
{
	Moe_PropBubbleGunStart = 0,
	Moe_PropBubbleGunWaitLoop = 1,
	Moe_PropBubbleGunLoop = 2,
	Moe_PropFBubbleGunEnd = 3,
	Moe_MAX = 4
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionPropFishing
enum class EMoeAnimModule_ActionPropFishing : uint8_t
{
	Moe_PropFishingThrow = 0,
	Moe_PropFishingWait = 1,
	Moe_PropFishingPull = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionVacuumCleaner
enum class EMoeAnimModule_ActionVacuumCleaner : uint8_t
{
	Moe_VacuumCleanerAim = 0,
	Moe_VacuumCleanerFire = 1,
	Moe_VacuumCleanerLoop = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionPropShoot
enum class EMoeAnimModule_ActionPropShoot : uint8_t
{
	Moe_PropShootAim = 0,
	Moe_PropShootFire = 1,
	Moe_PropShootLoop = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionAcceleration
enum class EMoeAnimModule_ActionAcceleration : uint8_t
{
	Moe_AccelerationStart = 0,
	Moe_AccelerationLoop = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionBounceFly
enum class EMoeAnimModule_ActionBounceFly : uint8_t
{
	Moe_BounceFlyStart = 0,
	Moe_BounceFlayLoop = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionOnFireworksState
enum class EMoeAnimModule_ActionOnFireworksState : uint8_t
{
	Moe_OnFireworksStart = 0,
	Moe_OnFireworksLoop = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionRotateHammerState
enum class EMoeAnimModule_ActionRotateHammerState : uint8_t
{
	Moe_RotateHammerStart = 0,
	Moe_RotateHammerLoop = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionBnbArrestedState
enum class EMoeAnimModule_ActionBnbArrestedState : uint8_t
{
	Moe_BnbArrestedStart = 0,
	Moe_BnbArrestedLoop = 1,
	Moe_BnbArrestedEnd = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionPoleVaultState
enum class EMoeAnimModule_ActionPoleVaultState : uint8_t
{
	Moe_PoleVaultStart = 0,
	Moe_PoleVaultLoop = 1,
	Moe_PoleVaultEnd = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionGlideState
enum class EMoeAnimModule_ActionGlideState : uint8_t
{
	Moe_GlideStart = 0,
	Moe_GlideLoop = 1,
	Moe_GlideEnd = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_ActionEasterEggShowState
enum class EMoeAnimModule_ActionEasterEggShowState : uint8_t
{
	Moe_Invalid = 0,
	Moe_EasterEggSwing = 1,
	Moe_Diving_SlideWay = 2,
	Moe_Diving_GettingUp_SlideWay = 3,
	Moe_DDP_BeGrabbedJump = 4,
	Moe_MAX = 5
};

// Object: Enum MoeGameCore.EMoeAnimModule_AnimPoseState
enum class EMoeAnimModule_AnimPoseState : uint8_t
{
	Moe_Locomotion = 0,
	Moe_Action_Body = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_BeGrabedMotionState
enum class EMoeAnimModule_BeGrabedMotionState : uint8_t
{
	Moe_BeGrabStart = 0,
	Moe_BeGrabLoop = 1,
	Moe_BeGrabEnd = 2,
	Moe_MAX = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_GrabActionState
enum class EMoeAnimModule_GrabActionState : uint8_t
{
	Moe_TryGrab = 0,
	Moe_GrabStart = 1,
	Moe_GrabLoop = 2,
	Moe_GrabThrow = 3,
	Moe_GrabEnd = 4,
	Moe_MAX = 5
};

// Object: Enum MoeGameCore.EMoeAnimModule_AnimDiveState_SlideWay
enum class EMoeAnimModule_AnimDiveState_SlideWay : uint8_t
{
	Dive_Start_Pipe = 0,
	Dive_Loop_Pipe = 1,
	Dive_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_AnimDiveState
enum class EMoeAnimModule_AnimDiveState : uint8_t
{
	Dive_Start = 0,
	Dive_Loop = 1,
	Dive_Fall = 2,
	Dive_Lang = 3,
	Dive_MAX = 4
};

// Object: Enum MoeGameCore.EMoeBaseMoveWithAvatarType
enum class EMoeBaseMoveWithAvatarType : uint8_t
{
	Moe_AvatarType_Default = 0,
	Moe_AvatarType = 1,
	Moe_AvatarType_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_IdleShowAnimAssetType
enum class EMoeAnimModule_IdleShowAnimAssetType : uint8_t
{
	Moe_IdleShow = 0,
	Moe_IdleShow = 1,
	Moe_IdleShow = 2,
	Max = 3
};

// Object: Enum MoeGameCore.EMoeAnimModule_DeadAnimAssetType
enum class EMoeAnimModule_DeadAnimAssetType : uint8_t
{
	Moe_Dead_Normal1_F = 0,
	Moe_Dead_Normal1_B = 1,
	Moe_Dead_Normal1_L = 2,
	Moe_Dead_Normal1_R = 3,
	Moe_Dead_Normal1_FL = 4,
	Moe_Dead_Normal1_FR = 5,
	Moe_Dead_Normal1_BL = 6,
	Moe_Dead_Normal1_BR = 7,
	Moe_Dead_Normal2_F = 8,
	Moe_Dead_Normal2_B = 9,
	Moe_Dead_Normal2_L = 10,
	Moe_Dead_Normal2_R = 11,
	Moe_Dead_Normal2_FL = 12,
	Moe_Dead_Normal2_FR = 13,
	Moe_Dead_Normal2_BL = 14,
	Moe_Dead_Normal2_BR = 15,
	Moe_Dead_ShotGun_F = 16,
	Moe_Dead_ShotGun_B = 17,
	Moe_Dead_ShotGun_L = 18,
	Moe_Dead_ShotGun_R = 19,
	Moe_Dead_ShotGun_FL = 20,
	Moe_Dead_ShotGun_FR = 21,
	Moe_Dead_ShotGun_BL = 22,
	Moe_Dead_ShotGun_BR = 23,
	Moe_Dead_Rocket_F = 24,
	Moe_Dead_Rocket_B = 25,
	Moe_Dead_Rocket_L = 26,
	Moe_Dead_Rocket_R = 27,
	Moe_Dead_Rocket_FL = 28,
	Moe_Dead_Rocket_FR = 29,
	Moe_Dead_Rocket_BL = 30,
	Moe_Dead_Rocket_BR = 31,
	Moe_Dead_HeadShot_F = 32,
	Moe_Dead_HeadShot_B = 33,
	Moe_Dead_HeadShot_L = 34,
	Moe_Dead_HeadShot_R = 35,
	Moe_Dead_HeadShot_FL = 36,
	Moe_Dead_HeadShot_FR = 37,
	Moe_Dead_HeadShot_BL = 38,
	Moe_Dead_HeadShot_BR = 39,
	Moe_Dead_Normal_F = 40,
	Moe_Dead_Normal_B = 41,
	Moe_Dead_Normal_L = 42,
	Moe_Dead_Normal_R = 43,
	Moe_Dead_Normal_FL = 44,
	Moe_Dead_Normal_FR = 45,
	Moe_Dead_Normal_BL = 46,
	Moe_Dead_Normal_BR = 47,
	Moe_Dead_MAX = 48
};

// Object: Enum MoeGameCore.EMoeAnimModule_JumpAnimAssetType
enum class EMoeAnimModule_JumpAnimAssetType : uint8_t
{
	Moe_Jump = 0,
	Moe_Jump = 1,
	Moe_Jump = 2,
	Moe_Jump = 3,
	Moe_Jump = 4,
	Max = 5,
	Moe_Jump_Ice = 6,
	Moe_Jump_Sprinted = 7,
	Moe_Jump_SlideWay = 8,
	Moe_DDP_Jump = 9,
	Moe_BioChase_Jump_Monster1002 = 10,
	Moe_BioChase_Jump_Monster1003 = 11,
	Moe_BioChase_Jump_Monster1004 = 12,
	Moe_Jump_OMD = 13,
	Moe_Jump_OMD = 14,
	Moe_Jump_OMD = 15,
	Moe_Jump_OMD_MagicBow = 16,
	Moe_Jump_OMD_MagicBow = 17,
	Moe_Jump_OMD_MagicBow = 18,
	Moe_Jump_OMD_Hammer = 19,
	Moe_Jump_OMD_Hammer = 20,
	Moe_Jump_OMD_Hammer = 21,
	Moe_Jump_OMD_Wand = 22,
	Moe_Jump_OMD_Wand = 23,
	Moe_Jump_OMD_Wand = 24,
	Moe_Jump_SlideRail = 25,
	Moe_NR3E3_FogboundNav_Jump = 26,
	Moe_Jump_Invalid = 27
};

// Object: Enum MoeGameCore.EMoeAnimModule_LocoGroundMoveStep
enum class EMoeAnimModule_LocoGroundMoveStep : uint8_t
{
	Moe_Step_Start = 0,
	Moe_Step_Loop = 1,
	Moe_Step_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_LocomotionWeaponEquipState
enum class EMoeAnimModule_LocomotionWeaponEquipState : uint8_t
{
	Moe_EmptyHand = 0,
	Moe_Equiped = 1,
	Moe_MAX = 2
};

// Object: Enum MoeGameCore.EMoeAnimModule_AnimBlendType
enum class EMoeAnimModule_AnimBlendType : uint8_t
{
	None = 0,
	BlendTwoHands = 1,
	BlendLeftHand = 2,
	BlendRightHand = 3,
	HandOnly = 4,
	EMoeAnimModule_MAX = 5
};

// Object: Enum MoeGameCore.EEmoteChangeMethod
enum class EEmoteChangeMethod : uint8_t
{
	DMI = 0,
	MPC = 1,
	EEmoteChangeMethod_MAX = 2
};

// Object: Enum MoeGameCore.ERedEnvelopeInteractType
enum class ERedEnvelopeInteractType : uint8_t
{
	TakeOut = 0,
	Putdown = 1,
	TakeBack = 2,
	ERedEnvelopeInteractType_MAX = 3
};

// Object: Enum MoeGameCore.EEuipPointType
enum class EEuipPointType : uint8_t
{
	Helmet = 0,
	Suit = 1,
	Clothes = 2,
	Bracelet = 3,
	Shoes = 4,
	BackAccessory = 5,
	CustomType1 = 6,
	CustomType2 = 7,
	CustomType3 = 8,
	CustomType4 = 9,
	CustomType5 = 10,
	CustomType6 = 11,
	CustomType7 = 12,
	CustomType8 = 13,
	CustomType9 = 14,
	CustomType10 = 15,
	Count = 16,
	EEuipPointType_MAX = 17
};

// Object: Enum MoeGameCore.EMoeAttrFinalChangeType
enum class EMoeAttrFinalChangeType : uint8_t
{
	EMA_SetFinalValue = 0,
	EMA_SetByScale = 1,
	EMA_MAX = 2
};

// Object: Enum MoeGameCore.EMoeChangeAttrType
enum class EMoeChangeAttrType : uint8_t
{
	UseBuff = 0,
	DirectChange = 1,
	EMoeChangeAttrType_MAX = 2
};

// Object: Enum MoeGameCore.EGameClientTag
enum class EGameClientTag : uint8_t
{
	ShutDownOnlyInClient = 1,
	EGameClientTag_MAX = 2
};

// Object: Enum MoeGameCore.EGamePointStampType
enum class EGamePointStampType : uint8_t
{
	InProgressStartStamp = 1,
	InProgressEndStamp = 2,
	EGamePointStampType_MAX = 3
};

// Object: Enum MoeGameCore.EMoeInteractiveActorState
enum class EMoeInteractiveActorState : uint8_t
{
	None = 0,
	Entry = 1,
	InGame = 2,
	Exit = 3,
	EMoeInteractiveActorState_MAX = 4
};

// Object: Enum MoeGameCore.EInteractiveRPCState
enum class EInteractiveRPCState : uint8_t
{
	None = 0,
	Gaming = 1,
	EInteractiveRPCState_MAX = 2
};

// Object: Enum MoeGameCore.EFPSRTPCType
enum class EFPSRTPCType : uint8_t
{
	Weapon = 0,
	Footstep = 1,
	UGCCustomWeapon = 2,
	EFPSRTPCType_MAX = 3
};

// Object: Enum MoeGameCore.EPlayerAnimType
enum class EPlayerAnimType : uint8_t
{
	None = 0,
	Free = 1,
	TPP = 2,
	FPP = 3,
	EPlayerAnimType_MAX = 4
};

// Object: Enum MoeGameCore.EQuitGameCode
enum class EQuitGameCode : uint8_t
{
	QUIT_BATTLE_CODE_INITIATIVE = 0,
	QUIT_BATTLE_CODE_LEAVE = 1,
	QUIT_BATTLE_CODE_FINISH_QUIT = 2,
	QUIT_BATTLE_CODE_NORMAL = 3,
	QUIT_BATTLE_CODE_KICKED = 4,
	QUIT_BATTLE_CODE_ACTIVE_LEAVE = 5,
	QUIT_BATTLE_CODE_MULTITEST_LEADER_QUIT = 6,
	QUIT_BATTLE_CODE_KICK_FROM_ROOM = 7,
	QUIT_BATTLE_CODE_MID_JOIN_FAIL = 8,
	QUIT_BATTLE_CODE_QUIT_WHEN_ENABLE_MID_JOIN = 9,
	QUIT_BATTLE_CODE_KICKED_WHEN_NOT_IN_BATTLE = 10,
	QUIT_BATTLE_CODE_RES_VERSION_CONFLICT = 11,
	QUIT_BATTLE_CODE_VERSION_ERROR = 12,
	NIL_CODE = 13,
	EQuitGameCode_MAX = 14
};

// Object: Enum MoeGameCore.EArmorType
enum class EArmorType : uint8_t
{
	Helmet = 0,
	Suit = 1,
	EArmorType_MAX = 2
};

// Object: Enum MoeGameCore.EArmorLevel
enum class EArmorLevel : uint8_t
{
	None = 0,
	LevelOne = 1,
	LevelTwo = 2,
	LevelThree = 3,
	LevelFour = 4,
	LevelFive = 5,
	EArmorLevel_MAX = 6
};

// Object: Enum MoeGameCore.EArmorStatus
enum class EArmorStatus : uint8_t
{
	Unequiped = 0,
	Equiped = 1,
	EArmorStatus_MAX = 2
};

// Object: Enum MoeGameCore.EBrGameItemSlot
enum class EBrGameItemSlot : uint8_t
{
	Unknown = 0,
	Normal = 1,
	MainWeapon = 2,
	SubWeapon = 3,
	MeleeWeapon = 4,
	Grenade1 = 5,
	Grenade2 = 6,
	LightAmmo = 7,
	HeavyAmmo = 8,
	ShotgunAmmo = 9,
	SniperAmmo = 10,
	RpgAmmo = 11,
	Medicine = 12,
	HeadShield = 13,
	BodyShield = 14,
	HPOverload = 15,
	DfGameBox = 16,
	DfGameBigBox = 17,
	AirDropBox = 18,
	WeaponAttachment = 19,
	Vehicle = 20,
	EBrGameItemSlot_MAX = 21
};

// Object: Enum MoeGameCore.EBrGameStage
enum class EBrGameStage : uint8_t
{
	Default = 0,
	Prepare = 1,
	WaitShrink = 2,
	Shrinking = 3,
	Reward = 4,
	EBrGameStage_MAX = 5
};

// Object: Enum MoeGameCore.EBrDeadMarkType
enum class EBrDeadMarkType : uint8_t
{
	None = 0,
	Effect = 1,
	Model = 2,
	EBrDeadMarkType_MAX = 3
};

// Object: Enum MoeGameCore.EBrGameMarkObjectType
enum class EBrGameMarkObjectType : uint8_t
{
	None = 0,
	Observe = 1,
	Enemy = 2,
	Supplies = 3,
	Attack = 4,
	Location = 5,
	EBrGameMarkObjectType_MAX = 6
};

// Object: Enum MoeGameCore.EMoeInteractInterruptReason
enum class EMoeInteractInterruptReason : uint8_t
{
	None = 0,
	Move = 1,
	Motion = 2,
	Action = 3,
	BeHit = 4,
	EMoeInteractInterruptReason_MAX = 5
};

// Object: Enum MoeGameCore.EMoeChatFailType
enum class EMoeChatFailType : uint8_t
{
	Cancel = 0,
	OverMaxDistance = 1,
	InviteeLogout = 2,
	EMoeChatFailType_MAX = 3
};

// Object: Enum MoeGameCore.EMoeChatMessageType
enum class EMoeChatMessageType : uint8_t
{
	RichText = 0,
	Emotion = 1,
	SpineEmotion = 2,
	BlueprintEmotion = 3,
	EMoeChatMessageType_MAX = 4
};

// Object: Enum MoeGameCore.ECharSmoothMoveMethod
enum class ECharSmoothMoveMethod : uint8_t
{
	None = 0,
	AddMovementInput = 1,
	Lerp = 2,
	ECharSmoothMoveMethod_MAX = 3
};

// Object: Enum MoeGameCore.ERoguelikeInteractiveType
enum class ERoguelikeInteractiveType : uint8_t
{
	Default = 0,
	Weapon = 1,
	Handbook = 2,
	GameStart = 3,
	Talent = 4,
	ERoguelikeInteractiveType_MAX = 5
};

// Object: Enum MoeGameCore.EMonsterState
enum class EMonsterState : uint8_t
{
	NONE = 0,
	IDLE = 1,
	WALK = 2,
	HIT = 3,
	DEAD = 4,
	SKILL = 5,
	SKILL = 6,
	SKILL = 7,
	SKILL = 8,
	SKILL = 9,
	RUN = 10,
	JUMP = 11,
	FALL = 12,
	HIT_ON_MOVE = 13,
	TYC_STATE_COUNT = 14,
	MIRE_WALK = 15,
	FIRE_WALK = 16,
	SPECIAL_BUFF = 17,
	SPECIAL_BUFF = 18,
	SPECIAL_BUFF = 19,
	SPECIAL_BUFF = 20,
	SPECIAL_BUFF = 21,
	SPECIAL_BUFF = 22,
	SPECIAL_BUFF = 23,
	SPECIAL_BUFF = 24,
	SPECIAL_BUFF = 25,
	SPECIAL_BUFF = 26,
	PHYSICS_FLY_START_B = 27,
	PHYSICS_FLY_LOOP_B = 28,
	PHYSICS_FLY_END_B = 29,
	PHYSICS_FLY_START_F = 30,
	PHYSICS_FLY_LOOP_F = 31,
	PHYSICS_FLY_END_F = 32,
	PHYSICS_FLY_START_L = 33,
	PHYSICS_FLY_LOOP_L = 34,
	PHYSICS_FLY_END_L = 35,
	PHYSICS_FLY_START_R = 36,
	PHYSICS_FLY_LOOP_R = 37,
	PHYSICS_FLY_END_R = 38,
	DEAD_FLY_START_U = 39,
	DEAD_FLY_LOOP_U = 40,
	DEAD_FLY_END_U = 41,
	DEAD_FLY_START_B = 42,
	DEAD_FLY_LOOP_B = 43,
	DEAD_FLY_END_B = 44,
	DEAD_FLY_START_F = 45,
	DEAD_FLY_LOOP_F = 46,
	DEAD_FLY_END_F = 47,
	DEAD_FLY_START_L = 48,
	DEAD_FLY_LOOP_L = 49,
	DEAD_FLY_END_L = 50,
	DEAD_FLY_START_R = 51,
	DEAD_FLY_LOOP_R = 52,
	DEAD_FLY_END_R = 53,
	SUCTION_START = 54,
	SUCTION_LOOP = 55,
	SUCTION_END = 56,
	MONSTE_STATE_COUNT = 57,
	TRAP_IDLE = 58,
	IDLE_TO_TRIGGER = 59,
	TRIGGER = 60,
	TRIGGER_TO_IDLE = 61,
	IDLE_OPEN = 62,
	BORN = 63,
	IDLE_TO_DEAD = 64,
	TRAP_DEAD = 65,
	DEAD_TO_IDLE = 66,
	RELOAD = 67,
	TRIGGER_TO_CD = 68,
	IDLE_CD = 69,
	START_BUILD = 70,
	EMonsterState_MAX = 71
};

// Object: Enum MoeGameCore.EStateMachineID
enum class EStateMachineID : uint8_t
{
	Default = 0,
	Archer = 1,
	Freeze = 2,
	ArcaneDragon = 3,
	Decoy = 4,
	GoldMelt = 5,
	StartBuild = 6,
	Reserved1 = 7,
	Reserved2 = 8,
	Reserved3 = 9,
	Reserved4 = 10,
	EStateMachineID_MAX = 11
};

// Object: Enum MoeGameCore.EActionType
enum class EActionType : uint8_t
{
	Jump = 0,
	Dive = 1,
	Sprint = 2,
	CreateBomb = 3,
	BeHit = 4,
	Rebirth = 5,
	UseProp = 6,
	PropHit = 7,
	UseSkill = 8,
	BeGrabbed = 9,
	EActionType_MAX = 10
};

// Object: Enum MoeGameCore.EMaterialParamType
enum class EMaterialParamType : uint8_t
{
	ParamFloat = 0,
	ParamVector = 1,
	EMaterialParamType_MAX = 2
};

// Object: Enum MoeGameCore.EDayNightState
enum class EDayNightState : uint8_t
{
	None = 0,
	Day = 1,
	Night = 2,
	EDayNightState_MAX = 3
};

// Object: Enum MoeGameCore.EDayNightRunType
enum class EDayNightRunType : uint8_t
{
	Auto = 0,
	ForceDay = 1,
	ForceNight = 2,
	ForceHide = 3,
	EDayNightRunType_MAX = 4
};

// Object: Enum MoeGameCore.EWeaponType
enum class EWeaponType : uint8_t
{
	WT_LFistLight = 0,
	WT_RFistLight = 1,
	WT_LFistHeavy = 2,
	WT_RFistHeavy = 3,
	WT_LFoot = 4,
	WT_RFoot = 5,
	WT_Head = 6,
	WT_MeleeWeaponLight = 7,
	WT_MeleeWeaponHeavy = 8,
	WT_RangeWeaponLight = 9,
	WT_ForSpecial = 10,
	WT_Electrocute = 11,
	WT_Push = 12,
	Max = 13
};

// Object: Enum MoeGameCore.EDDPGrabControllType
enum class EDDPGrabControllType : uint8_t
{
	ePAGCLeft = 0,
	ePAGCRight = 1,
	EDDPGrabControllType_MAX = 2
};

// Object: Enum MoeGameCore.EDDPTimeSyncType
enum class EDDPTimeSyncType : uint8_t
{
	DDPTimeSync_TimeStamp = 0,
	DDPTimeSync_DeltaTime = 1,
	DDPTimeSync_MAX = 2
};

// Object: Enum MoeGameCore.EPAWeaponEquipType
enum class EPAWeaponEquipType : uint8_t
{
	E_PA_Weapon_Equip_Empty = 0,
	E_PA_Weapon_Equip_Hammer = 1,
	E_PA_Weapon_Equip_Branch = 2,
	E_PA_Weapon_Equip_PineCone = 3,
	E_PA_Weapon_Equip_Pan = 4,
	E_PA_Weapon_Equip_Chomper = 5,
	E_PA_Weapon_Equip_Electrocute_Gun = 6,
	E_PA_Weapon_Equip_Freeze_Gun = 7,
	E_DDP_Weapon_Equip_PalmLeafFan = 8,
	E_MAX = 9
};

// Object: Enum MoeGameCore.EDfGameResult
enum class EDfGameResult : uint8_t
{
	UnFinish = 0,
	EscapeSuccess = 1,
	EscapeFail = 2,
	EDfGameResult_MAX = 3
};

// Object: Enum MoeGameCore.EDfGameEscapePointStatus
enum class EDfGameEscapePointStatus : uint8_t
{
	None = 0,
	TeammateEscaping = 1,
	EnemyEscaping = 2,
	EDfGameEscapePointStatus_MAX = 3
};

// Object: Enum MoeGameCore.EDfGameEscapeState
enum class EDfGameEscapeState : uint8_t
{
	NoEscape = 0,
	Escaping = 1,
	InPointCantEscape = 2,
	Escaped = 3,
	EDfGameEscapeState_MAX = 4
};

// Object: Enum MoeGameCore.EDfGameStage
enum class EDfGameStage : uint8_t
{
	Default = 0,
	Prepare = 1,
	Playing = 2,
	Reward = 3,
	EDfGameStage_MAX = 4
};

// Object: Enum MoeGameCore.EDisplayPhase
enum class EDisplayPhase : uint8_t
{
	EnterGame = 0,
	GameOut = 1,
	EDisplayPhase_MAX = 2
};

// Object: Enum MoeGameCore.EMoePlayerTaskStatus
enum class EMoePlayerTaskStatus : uint8_t
{
	Running = 0,
	FinishTask = 1,
	LostTask = 2,
	MAX = 3
};

// Object: Enum MoeGameCore.EMoePlayerResultStatus
enum class EMoePlayerResultStatus : uint8_t
{
	WaitEndResult = 0,
	Eliminated = 1,
	Victory = 2,
	Draw = 3,
	MAX = 4
};

// Object: Enum MoeGameCore.EAddDirection
enum class EAddDirection : uint8_t
{
	Forward = 0,
	Right = 1,
	Up = 2,
	All = 3,
	EAddDirection_MAX = 4
};

// Object: Enum MoeGameCore.EGroupParamType
enum class EGroupParamType : uint8_t
{
	RotateSpeed = 0,
	ScaleWaitTime = 1,
	DelayTime = 2,
	RealColorType = 3,
	CountDown = 4,
	ColorType = 5,
	MoveTime = 6,
	Multi = 7,
	ImpulseValue = 8,
	HuddlePlayerNum = 9,
	Difficult = 10,
	Enable = 11,
	Opacity = 12,
	DurationTime = 13,
	TargetHeight = 14,
	DeltaHeight = 15,
	IndividualColorRoundTime = 16,
	SpawnActorType = 17,
	SpawnActorCount = 18,
	SpawnActorLifeTime = 19,
	PositionOffset = 20,
	WaterWarningTime = 21,
	RobotFaceIndex = 22,
	LightingEmissIndex = 23,
	IndividualMatSwitch = 24,
	FaceColorIndex = 25,
	RobotMatIndex = 26,
	bOpenDecoLamp = 27,
	IndividualColorObsoleteRate = 28,
	MeshAnimState = 29,
	SnakeTriggerMoveDuration = 30,
	PlayParticleByIndex = 31,
	CurColorIndex = 112,
	CurColorDataIndex = 113,
	MuTouRen = 128,
	EGroupParamType_MAX = 129
};

// Object: Enum MoeGameCore.EGroupActionType
enum class EGroupActionType : uint8_t
{
	Gear_Show = 0,
	Gear_Hidden = 1,
	Memory_ShowRealColorAndResume = 2,
	MemoryPlat_HiddenAndShow = 3,
	MemoryScreen_ShowCountDown = 4,
	Gear_Destroy = 5,
	Laser_InitMove = 6,
	BlastBall_Explode = 7,
	Plat_SpawnProp = 8,
	Plat_RunFunc = 9,
	Plat_PauseFunc = 10,
	CheckNeedEnable = 11,
	Move2TargetHeight = 12,
	AddHeight = 13,
	LauncherStart = 14,
	Launcher_ShowRealActorAndResume = 15,
	ResetGoalMahjong = 16,
	SetIndividualPawnColor = 17,
	IndividualColorRoundEnd = 18,
	SnakeTriggerLaunch = 19,
	SnakeTriggerDestroy = 20,
	SnakeTriggerBeginTip = 21,
	SnakeTriggerEndTip = 22,
	MemoryScreen_ShowTips = 23,
	MemoryScreen_HideTips = 24,
	Global_OpenCountdown = 224,
	Global_OpenMessageTip = 225,
	Global_PlaySfxById = 227,
	Global_SnakeAddLength = 228,
	Global_SnakeAddSpeed = 229,
	EGroupActionType_MAX = 230
};

// Object: Enum MoeGameCore.EMoeSkillPanelAbility
enum class EMoeSkillPanelAbility : uint8_t
{
	Prop = 0,
	InviteTeleport = 1,
	Grab = 2,
	Sprint = 3,
	Dive = 4,
	Jump = 5,
	EMoeSkillPanelAbility_MAX = 6
};

// Object: Enum MoeGameCore.EEmoteType
enum class EEmoteType : uint8_t
{
	OpenEyes = 0,
	CloseEyes = 1,
	Happy = 2,
	Sad = 3,
	ExpEmote01 = 4,
	ExpEmote02 = 5,
	ExpEmote03 = 6,
	ExpEmote04 = 7,
	ExpEmote05 = 8,
	ExpEmote06 = 9,
	ExpEmote07 = 10,
	EEmoteType_MAX = 11
};

// Object: Enum MoeGameCore.EDDPStateParamType
enum class EDDPStateParamType : uint8_t
{
	ePASP_None = 0,
	ePASP_StunImpluse = 1,
	ePASP_ElectroCute = 2,
	ePASP_Max = 3
};

// Object: Enum MoeGameCore.EMoeCharDeathType
enum class EMoeCharDeathType : uint8_t
{
	None = 0,
	Fall = 1,
	Shot = 2,
	Melee = 3,
	EnemyRemote = 4,
	PoliceRemote = 5,
	Self = 6,
	Reset = 7,
	EMoeCharDeathType_MAX = 8
};

// Object: Enum MoeGameCore.EMoeCharSkillType
enum class EMoeCharSkillType : uint8_t
{
	None = 0,
	Jump = 1,
	Dive = 2,
	Grab = 3,
	Sprint = 4,
	Melee = 5,
	Scan = 6,
	Search = 7,
	Morph = 8,
	Cloaking = 9,
	Rescue = 10,
	FPSSnipeOpenAim = 11,
	FPSSnipeCloseAim = 12,
	FPSAttact = 13,
	FPSReammo = 14,
	FPSChangeView_1P = 15,
	FPSChangeView_3P = 16,
	FPSChangeShootModel_Auto = 17,
	FPSChangeShootModel_Manual = 18,
	FPSSettingOpen = 19,
	TeleportToTeammate = 20,
	Follow = 21,
	FPSSaveTeammate = 22,
	FPSSprint = 23,
	UGCSkill = 24,
	FPSOpenIronSights = 25,
	FPSCloseIronSights = 26,
	EMoeCharSkillType_MAX = 27
};

// Object: Enum MoeGameCore.EMoeCharBounceState
enum class EMoeCharBounceState : uint8_t
{
	SpringBoard_Bounce = 0,
	Trampoline_Bounce = 1,
	EMoeCharBounceState_MAX = 2
};

// Object: Enum MoeGameCore.EMoeCharPAState
enum class EMoeCharPAState : uint8_t
{
	None = 0,
	Grab = 1,
	Attack = 2,
	BeAttacked = 3,
	Ragdoll = 4,
	SoftGrab = 5,
	Max = 231
};

// Object: Enum MoeGameCore.EMoeCharStateLayer
enum class EMoeCharStateLayer : uint8_t
{
	Action = 0,
	Motion = 1,
	Anim = 2,
	PostProcess = 3,
	Stage = 4,
	EMoeCharStateLayer_MAX = 5
};

// Object: Enum MoeGameCore.ECharStateOperation
enum class ECharStateOperation : uint8_t
{
	CSO_Coexist = 0,
	CSO_Replace = 1,
	CSO_Blocked = 2,
	CSO_Interrupt = 3,
	CSO_MAX = 4
};

// Object: Enum MoeGameCore.ECharOGCMotionState
enum class ECharOGCMotionState : uint8_t
{
	InValid = 0,
	LeafFly = 1,
	MovingCard = 2,
	Snake = 3,
	DriverTank = 4,
	FlappyBird = 5,
	SlideRail = 6,
	Max = 7
};

// Object: Enum MoeGameCore.ECharOGCActionState
enum class ECharOGCActionState : uint8_t
{
	InValid = 0,
	GrabBasketBall = 1,
	GhostStand = 2,
	GrabCollectionCoin = 3,
	Max = 4
};

// Object: Enum MoeGameCore.ECharPostProcessState
enum class ECharPostProcessState : uint8_t
{
	None = 0,
	Dead = 1,
	InPoison = 2,
	Max = 3
};

// Object: Enum MoeGameCore.EArenaActionState
enum class EArenaActionState : uint8_t
{
	Arena_OnHit = 0,
	Max = 1
};

// Object: Enum MoeGameCore.EMoeEffectType
enum class EMoeEffectType : uint8_t
{
	ChangeFloatAttr = 0,
	ChangeStringAttr = 1,
	ChangeBoolAttr = 2,
	ChangeIntAttr = 3,
	AddToMovement = 4,
	EMoeEffectType_MAX = 5
};

// Object: Enum MoeGameCore.EMoeIaResCode
enum class EMoeIaResCode : uint8_t
{
	Success = 0,
	IsBusy = 1,
	NoActor = 2,
	Other = 3,
	EMoeIaResCode_MAX = 4
};

// Object: Enum MoeGameCore.EMoeIaOperation
enum class EMoeIaOperation : uint8_t
{
	Enter = 0,
	Exit = 1,
	EMoeIaOperation_MAX = 2
};

// Object: Enum MoeGameCore.EMoeCompareType
enum class EMoeCompareType : uint8_t
{
	Equal = 0,
	Less = 1,
	LessEqual = 2,
	Greater = 3,
	GreaterEqual = 4,
	NotEqual = 5,
	EMoeCompareType_MAX = 6
};

// Object: Enum MoeGameCore.EMoeVarType
enum class EMoeVarType : uint8_t
{
	Float = 0,
	Bool = 1,
	Vector = 2,
	Int = 3,
	String = 4,
	Class = 5,
	Object = 6,
	Particle = 7,
	EMoeVarType_MAX = 8
};

// Object: Enum MoeGameCore.EObjectParamType
enum class EObjectParamType : uint8_t
{
	RuntimeGet = 0,
	SelfActor = 1,
	SelfComponent = 2,
	EObjectParamType_MAX = 3
};

// Object: Enum MoeGameCore.EStringParamType
enum class EStringParamType : uint8_t
{
	Constant = 0,
	RuntimeGet = 1,
	Append = 2,
	GetName = 3,
	EStringParamType_MAX = 4
};

// Object: Enum MoeGameCore.EVectorParamType
enum class EVectorParamType : uint8_t
{
	Constant = 0,
	RuntimeGet = 1,
	XYZ = 2,
	GetPosition = 3,
	GetVelocity = 4,
	GetDirection = 5,
	ScaleAndOffset = 6,
	FromSpline = 7,
	EVectorParamType_MAX = 8
};

// Object: Enum MoeGameCore.EFloatParamType
enum class EFloatParamType : uint8_t
{
	Constant = 0,
	RuntimeGet = 1,
	Operate2Float = 2,
	VectorSize = 3,
	EFloatParamType_MAX = 4
};

// Object: Enum MoeGameCore.EBoolParamType
enum class EBoolParamType : uint8_t
{
	Constant = 0,
	RuntimeGet = 1,
	BoolLogic = 2,
	CompareFloat = 3,
	EBoolParamType_MAX = 4
};

// Object: Enum MoeGameCore.EMoeParamType
enum class EMoeParamType : uint8_t
{
	Invalid = 0,
	Action = 1,
	BoolParam = 2,
	FloatParam = 3,
	VectorParam = 4,
	StringParam = 5,
	ObjectParam = 6,
	EMoeParamType_MAX = 7
};

// Object: Enum MoeGameCore.EMoeBlockSightType
enum class EMoeBlockSightType : uint8_t
{
	Default = 0,
	None = 1,
	Transparent = 2,
	Sketch = 3,
	Indentation = 4,
	EMoeBlockSightType_MAX = 5
};

// Object: Enum MoeGameCore.EMoePlaceableType
enum class EMoePlaceableType : uint8_t
{
	Default = 0,
	LogicSwitch = 1,
	Ground = 2,
	Decorate = 3,
	Gear = 4,
	Prop = 5,
	Item = 6,
	Logic = 7,
	EMoePlaceableType_MAX = 8
};

// Object: Enum MoeGameCore.ETriggerAreaType
enum class ETriggerAreaType : uint32_t
{
	ETAT_None = 0,
	ETAT_Player = 1,
	ETAT_PropActor = 2,
	ETAT_MoveActor = 4,
	ETAT_SpecifiedActor = 8,
	ETAT_Monster = 16,
	ETAT_NPC = 32,
	ETAT_Vehicle = 64,
	ETAT_AllPA = 65535,
	ETAT_MAX = 65536
};

// Object: Enum MoeGameCore.EMoeUniformScaleType
enum class EMoeUniformScaleType : uint8_t
{
	None = 0,
	XYZ = 1,
	XY = 2,
	XZ = 3,
	YZ = 4,
	EMoeUniformScaleType_MAX = 5
};

// Object: Enum MoeGameCore.EMoeNetAffectRoleRule
enum class EMoeNetAffectRoleRule : uint8_t
{
	NARR_Never = 0,
	NARR_L_Autonomous_R_Authority = 1,
	NARR_L_Authority_R_Autonomous = 2,
	NARR_L_Authority_R_Simulated = 4,
	NARR_L_Simulated_R_Authority = 8,
	NARR_MAX = 9
};

// Object: Enum MoeGameCore.EMoeNetActivateMode
enum class EMoeNetActivateMode : uint8_t
{
	MoeNetActivateMode_Never = 0,
	MoeNetActivateMode_Always = 1,
	MoeNetActivateMode_OnlyClient = 2,
	MoeNetActivateMode_OnlyServer = 3,
	MoeNetActivateMode_MAX = 4
};

// Object: Enum MoeGameCore.EFPSAIType
enum class EFPSAIType : uint8_t
{
	FPSAI_None = 0,
	FPSAI_BIO_Human = 1,
	FPSAI_BIO_Monster = 2,
	FPSAI_MAX = 3
};

// Object: Enum MoeGameCore.EInteractResCode
enum class EInteractResCode : uint8_t
{
	Success = 0,
	IsBusy = 1,
	NoActor = 2,
	Other = 3,
	EInteractResCode_MAX = 4
};

// Object: Enum MoeGameCore.ERelationalOperator
enum class ERelationalOperator : uint8_t
{
	Equal = 0,
	LessThan = 1,
	Bigger = 2,
	LessOrEqual = 3,
	BiggerOrEqual = 4,
	NotEqual = 5,
	ERelationalOperator_MAX = 6
};

// Object: Enum MoeGameCore.EPlayerInfoTiming
enum class EPlayerInfoTiming : uint8_t
{
	None = 0,
	GameBeginPlay = 1,
	EPlayerInfoTiming_MAX = 2
};

// Object: Enum MoeGameCore.EGameInfoTiming
enum class EGameInfoTiming : uint8_t
{
	None = 0,
	GameBeginPlay = 1,
	EGameInfoTiming_MAX = 2
};

// Object: Enum MoeGameCore.EGameTiming
enum class EGameTiming : uint8_t
{
	None = 0,
	GameBeginPlay = 1,
	GameBeforeInit = 2,
	GameAfterInit = 3,
	GameBeforeEnterState = 4,
	GameAfterEnterState = 5,
	GameClientPostActorCreator = 6,
	GameClientBeginPlay = 7,
	GameClientBeforeBindPlayerInfo = 8,
	GameClientBeforeBindGameInfo = 9,
	GameClientSpawnedFromFactory = 10,
	GameSpawnedFromFactory = 11,
	EGameTiming_MAX = 12
};

// Object: Enum MoeGameCore.EUGCPlayerTaskStatus
enum class EUGCPlayerTaskStatus : uint8_t
{
	Running = 0,
	FinishTask = 1,
	LostTask = 2,
	MAX = 3
};

// Object: Enum MoeGameCore.EUGCPlayerResultStatus
enum class EUGCPlayerResultStatus : uint8_t
{
	WaitEndResult = 0,
	Eliminated = 1,
	Victory = 2,
	Draw = 3,
	MAX = 4
};

// Object: Enum MoeGameCore.EGameType
enum class EGameType : uint8_t
{
	Undefined = 0,
	URunningGame = 1,
	USurvivalGame = 2,
	UPointsGame = 3,
	UCampGame = 4,
	UGCEditorGame = 5,
	UGCPlayGame = 6,
	TrailPlayGame = 7,
	URelayOneByOneRaceGame = 8,
	UCommunityGame = 9,
	UDDP = 10,
	UAITrainGame = 11,
	UIntactStadaloneGame = 12,
	HomeGame = 13,
	HomeEditorGame = 14,
	GameModeID_GunGame = 24,
	GameModeID_BioChase = 25,
	GameModeID_Roguelike = 26,
	GameModeID_NR3E2 = 103,
	GameModeID_NR3E1 = 104,
	GameModeID_NR3E3 = 105,
	All = 106,
	EGameType_MAX = 107
};

// Object: Enum MoeGameCore.EGameStatus
enum class EGameStatus : uint8_t
{
	UnInitialized = 0,
	Initialized = 1,
	Running = 2,
	Paused = 3,
	GameOver = 4,
	Account = 5,
	EGameStatus_MAX = 6
};

// Object: Enum MoeGameCore.EFeatureSystemLoadTiming
enum class EFeatureSystemLoadTiming : uint8_t
{
	ManualInitialize = 0,
	WorldInitialization = 1,
	PostWorldInitialization = 2,
	WorldInitializedActors = 3,
	RootGamePrepare = 5,
	GameInitialize = 6,
	AutoInitialize = 99,
	All = 100,
	EFeatureSystemLoadTiming_MAX = 101
};

// Object: Enum MoeGameCore.EFeatureSystemManagerStage
enum class EFeatureSystemManagerStage : uint8_t
{
	Default = 0,
	Initialization = 1,
	PostWorldInitialization = 2,
	WorldInitializedActors = 3,
	PreRootGameCreate = 4,
	PostRootGameCreated = 5,
	DeInitialize = 6,
	EFeatureSystemManagerStage_MAX = 7
};

// Object: Enum MoeGameCore.EGameInfoWaitTaskFinishType
enum class EGameInfoWaitTaskFinishType : uint8_t
{
	Cancel = 0,
	Finish = 1,
	EGameInfoWaitTaskFinishType_MAX = 2
};

// Object: Enum MoeGameCore.ECompressionType
enum class ECompressionType : uint8_t
{
	NormalCompression = 0,
	MixedCompression = 1,
	UltimateCompression = 2,
	LossCompression = 3,
	ECompressionType_MAX = 4
};

// Object: Enum MoeGameCore.EDDPSyncDataDef
enum class EDDPSyncDataDef : uint8_t
{
	E_PartyAnimSyncDataDef_Transform = 0,
	E_PartyAnimSyncDataDef_AnimState = 1,
	E_PartyAnimSyncDataDef_Montage = 2,
	E_PartyAnimSyncDataDef_CharacterState = 3,
	E_PartyAnimSyncDataDef_OnHit = 4,
	E_PartyAnimSyncDataDef_SwitchWeapon = 5,
	E_PartyAnimSyncDataDef_TransformWithBone = 6,
	E_PartyAnimSyncDataDef_FireProjectile = 7,
	E_PartyAnimSyncDataDef_WeaponVisible = 8,
	E_PartyAnimSyncDataDef_TimeFreeze = 9,
	E_PartyAnimSyncDataDef_ParticlePlay = 10,
	E_PartyAnimSyncDataDef_AudioPlay = 11,
	E_PartyAnimSyncDataDef_DataFlow = 12,
	E_DDPSyncDataDef_Visibility = 13,
	E_DDPSyncDataDef_CountDown = 14,
	E_DDPSyncDataDef_ForceStateSync = 15,
	E_DDPSyncDataDef_PartyclePlayLocal = 16,
	E_DDPSyncDataDef_UGCPhyXVehicle = 17,
	E_DDPSyncDataDef_DestructionPhase = 18,
	E_MAX = 19
};

// Object: Enum MoeGameCore.EPlaceableSubType
enum class EPlaceableSubType : uint8_t
{
	DEFAULT = 0,
	SAMPLE = 1,
	ITEMBASE = 2,
	HUDDLEPLAT = 3,
	HUDDLESCREEN = 4,
	GROUNDBASE = 5,
	DECORATEBASE = 6,
	LOGICACTORBASE = 7,
	ACCELERATOR = 8,
	AUTODOOR = 9,
	AWARDRING = 10,
	BARRIER = 11,
	BEAN = 12,
	BENGCHUANG = 13,
	BLASTBALLOON = 14,
	BLOWPLAT = 15,
	BOOST = 16,
	BOUNCE = 17,
	BROKENCUBE = 18,
	BROKENPLAT = 19,
	BULLETLAUNCHER = 20,
	BUMPER = 21,
	BUTTON = 22,
	CHASINGLIGHT = 23,
	CIRCLEHIT = 24,
	COLORINGFLOOR = 25,
	CONVEYOR = 26,
	CROSSGEAR = 27,
	DISC = 28,
	EXPLODEDPLAT = 29,
	FAKEDOOR = 30,
	FANS = 31,
	FIST = 32,
	FROG = 33,
	FUNNEL = 34,
	GRAVITY = 35,
	ROTATIONHAMMER = 36,
	HIDEGRASS = 37,
	LASER = 38,
	LIFTINGPLAT = 39,
	LIGHTPLATS = 40,
	LILYPAD = 41,
	MEMORYPLAT = 42,
	MOTIANLUN = 43,
	MOVEBAR = 44,
	MOVEPLAT = 45,
	MOVEWOOD = 46,
	HITMUSICPLAT = 47,
	NOTICEBOARD = 48,
	PORTAL = 49,
	PROPELLER = 50,
	PUSHDOOR = 51,
	QIUQIAN = 52,
	RAFT = 53,
	RINGDOOR = 54,
	ROLLER = 55,
	ROTATEBAR = 56,
	ROTATEPLAT = 57,
	SEESAWPLAT = 58,
	SHENGJIANGTAI = 59,
	SLIDE = 60,
	NOCOLLISIONFLOOR = 61,
	SPLINESLIDEPIPELINE = 62,
	SPRINGBOARD = 63,
	SWINGBALL = 64,
	TIMINGDROPFLOOR = 65,
	TIMINGPLAT = 66,
	TRANSCOLORPLAT = 67,
	TUBE = 68,
	TURNTABLE = 69,
	WATER = 70,
	RHYTHMBRIDGE = 71,
	CRAZYFACTROYPROP = 72,
	WALLCOMINGMOVE = 73,
	PROPSPAWNER = 74,
	SLIDEWAY = 75,
	TRAP = 76,
	HOLE = 77,
	BELL = 78,
	DEADBODY = 79,
	WAYPOINT = 80,
	PLACECLOUD = 81,
	EndPointPodium = 82,
	Fountain = 83,
	SafeZone = 84,
	SlidewayTrigger = 85,
	Robot = 86,
	MahjongScoreArea = 87,
	BnbBomb = 88,
	BnbFloor = 89,
	CircularButton = 90,
	MoeIndividualColorLevelConfig = 91,
	DianWang = 92,
	PlasmaBallFixed = 93,
	PortalTargetPoint = 94,
	RandomPortal = 95,
	PenguinScoreArea = 96,
	MoeCommonGear = 97,
	UGCWeapon = 98,
	UGCMeleeWeapon = 99,
	UGCOMDSpawnPoint = 100,
	UGCOMDRebirthPoint = 101,
	DDP_MountainTop_Floor = 102,
	DDP_MountainTop_Bomb = 103,
	DDP_MountainTop_Grass = 104,
	DDP_MountainTop_Rock = 105,
	BALLS = 106,
	BOMBS = 107,
	BOXES = 108,
	BUCKETS = 109,
	DICE = 110,
	FIRECRACKER = 111,
	FOOTBALL = 112,
	SNOOKER = 113,
	STARS = 114,
	QUINCUNCIALPILES = 115,
	ACTORLAUNCHER = 116,
	OTHER = 117,
	BP = 118,
	CROWNSTAGES = 119,
	GAMEHIDDEN = 120,
	LOGICACTOR = 121,
	MODIFIER = 122,
	PROPBOX = 123,
	SPAWNERS = 124,
	TRIGGER = 125,
	FPS_BIO_TRIGGER = 126,
	FPS_BIO_MONSTERUPGRADE = 127,
	FPS_BIO_BULLETBOX = 128,
	FPS_FL_RPG = 129,
	FPS_BR_POISON_CIRCLE = 130,
	FPS_BR_ITEM_POINT = 131,
	PA_SAFE_AREA = 132,
	PROPBASEBALL = 133,
	PROPBASEBANANA = 134,
	PROPBASEBOMB = 135,
	PROPBASEBOOMERANG = 136,
	PROPBASECHANGESIZE = 137,
	PROPBASECIRCLEHIT = 138,
	PROPBASECLOUD = 139,
	PROPBASEEASTEREGG = 140,
	PROPBASEFORK = 141,
	PROPBASEGLIDER = 142,
	PROPBASEICE = 143,
	PROPBASEJETPACK = 144,
	PROPBASELANDMINE = 145,
	PROPBASEMAGNET = 146,
	PROPBASEPOLE = 147,
	PROPBASEROADBLOCK = 148,
	PROPBASECHICKEN = 149,
	PROPBASESPEEDUP = 150,
	PROPBASESPRINGBOARD = 151,
	PROPBASEMAGICCUBE = 152,
	PROPBASEFISHING = 153,
	PROPBASEBNBPOWER = 154,
	PROPBASEFIREWORK = 155,
	PROPBASEFPSGUN = 156,
	PROPBASERANDOMPORTAL = 157,
	PROPBASEMUCUS = 158,
	PROPBASEFIREWORKS = 159,
	PROPBASEBNBCOUNT = 160,
	PROPBASEEASTEREGGATTACH = 161,
	PROPBASEDYECANNON = 162,
	PROPBASEBULLETMAXCOUNT = 163,
	PROPBASEROCKETLAUNCHER = 164,
	PROPBASEFIREWORKLAUNCHER = 165,
	PROPBASEC4BOMB = 166,
	PROPBASEFPSCUSTOMEQUIP = 167,
	PROPBASEMUCUSTRAP = 168,
	PROPBASESTEALTHBOOM = 169,
	TYC_TD_SKILL = 170,
	NR3E_E1_JETPACK = 171,
	NR3E_E1_BOMB = 172,
	NR3E_E1_LANDMINE = 173,
	NR3E_E1_DETECT = 174,
	NR3E_E1_MAGNET = 175,
	NR3E_E1_SHIELD = 176,
	NR3E_E1_DUPLICATE = 177,
	NR3E_E1_MIMICRY = 178,
	NR3E_E1_GRAFFITI = 179,
	NR3E_E1_PORTAL = 180,
	NR3E_E1_TRAP = 181,
	NR3E_E1_SNOWBALL = 182,
	NR3E_E1_SPIRIT = 183,
	NR3E_E1_CLOUD = 184,
	NR3E_E1_GIANT = 185,
	NR3E_E1_SUMMON_CLOUD = 186,
	NR3E_E1_SUMMON_SNOWBALL = 187,
	Arena_WOODEN_BOX = 188,
	Arena_TREASURE_BOX = 189,
	Arena_CARD = 190,
	Arena_PORTAL = 191,
	Arena_CARD_BLUE = 192,
	Arena_CARD_PURPLE = 193,
	Arena_CARD_CHROMA = 194,
	Arena_OCCUPIED = 195,
	Arena_BOSS_WARNING = 196,
	Arena_DUMMY = 197,
	Arena_Iron_TREASURE_BOX = 198,
	Arena_Launch_Device = 199,
	Arena_Hok_TowerCoin = 200,
	Arena_Hok_SupportCloth = 201,
	Arena_Hok_RecoverFlowerFruit = 202,
	Arena_Hok_Prop = 203,
	Arena_Hok_Side_Door = 204,
	Arena_Hok_Prop_Gemstone = 205,
	Arena_FB_BouncingWall = 206,
	Arena_FB_Goal = 207,
	Arena_FB_HalfCourt = 208,
	Arena_FB_HealingArea = 209,
	Arena_FB_RevivePoint = 210,
	Arena_FB_Ball = 211,
	Arena_HaiYueWorld_Pos = 212,
	Arena_FB_BoundBox = 213,
	Arena_Hok_Occupied_Dragon = 214,
	Arena_Hok_Occupied_Goat = 215,
	EPlaceableSubType_MAX = 216
};

// Object: Enum MoeGameCore.EPlaceableShape
enum class EPlaceableShape : uint8_t
{
	Box = 0,
	Capsule = 1,
	Sphere = 2,
	Cylinder = 3,
	Ring = 4,
	EPlaceableShape_MAX = 5
};

// Object: Enum MoeGameCore.EPlaceableType
enum class EPlaceableType : uint8_t
{
	Default = 0,
	BLOCK = 8,
	Item = 9,
	EPlaceableType_MAX = 10
};

// Object: Enum MoeGameCore.EMAYCameraType
enum class EMAYCameraType : uint8_t
{
	Default = 0,
	Normal = 1,
	Spectator = 2,
	InDungeon = 3,
	EMAYCameraType_MAX = 4
};

// Object: Enum MoeGameCore.EMCGCameraType
enum class EMCGCameraType : uint8_t
{
	Default = 0,
	Character = 1,
	Boss = 2,
	EMCGCameraType_MAX = 3
};

// Object: Enum MoeGameCore.EAnimModule_MonsterSkill
enum class EAnimModule_MonsterSkill : uint8_t
{
	MM_SkillStart = 0,
	MM_SkillLoop = 1,
	MM_SkillEnd = 2,
	MM_MAX = 3
};

// Object: Enum MoeGameCore.EAnimModule_MonsterJumpDown
enum class EAnimModule_MonsterJumpDown : uint8_t
{
	MM_JumpDownStart = 0,
	MM_JumpDownLoop = 1,
	MM_JumpDownEnd = 2,
	MM_MAX = 3
};

// Object: Enum MoeGameCore.EAnimModule_MonsterSlide
enum class EAnimModule_MonsterSlide : uint8_t
{
	MM_SlideStart = 0,
	MM_SlideLoop = 1,
	MM_SlideEnd = 2,
	MM_MAX = 3
};

// Object: Enum MoeGameCore.EAnimModule_MonsterJump
enum class EAnimModule_MonsterJump : uint8_t
{
	MM_JumpStart = 0,
	MM_JumpLoop = 1,
	MM_JumpEnd = 2,
	MM_MAX = 3
};

// Object: Enum MoeGameCore.EAnimModule_MonsterDizzy
enum class EAnimModule_MonsterDizzy : uint8_t
{
	MM_DizzyStart = 0,
	MM_DizzyLoop = 1,
	MM_DizzyEnd = 2,
	MM_MAX = 3
};

// Object: Enum MoeGameCore.EAnimModule_MonsterActionState
enum class EAnimModule_MonsterActionState : uint8_t
{
	MM_Idle = 0,
	MM_InGame = 1,
	MM_Move = 2,
	MM_Attack = 3,
	MM_BeAttack = 4,
	MM_SearchPlayer = 5,
	MM_PlayerFound = 6,
	MM_GetPower = 7,
	MM_Dizzy = 8,
	MM_Rush = 9,
	MM_Jump = 10,
	MM_Slide = 11,
	MM_JumpDown = 12,
	MM_Skill = 13,
	MM_Death = 14,
	MM_AfterAttack = 15,
	MM_Anim101 = 101,
	MM_Anim102 = 102,
	MM_Anim103 = 103,
	MM_Anim104 = 104,
	MM_Anim105 = 105,
	MM_Anim106 = 106,
	MM_Anim107 = 107,
	MM_Anim108 = 108,
	MM_Anim109 = 109,
	MM_Anim110 = 110,
	MM_Anim111 = 111,
	MM_Anim112 = 112,
	MM_Anim113 = 113,
	MM_Anim114 = 114,
	MM_Anim115 = 115,
	MM_Anim116 = 116,
	MM_Anim117 = 117,
	MM_Anim118 = 118,
	MM_Anim119 = 119,
	MM_Anim120 = 120,
	MM_MAX = 121
};

// Object: Enum MoeGameCore.EMobileInputPhase
enum class EMobileInputPhase : uint8_t
{
	MobileInputPhase_Began = 0,
	MobileInputPhase_Moved = 1,
	MobileInputPhase_Stationary = 2,
	MobileInputPhase_Ended = 3,
	MobileInputPhase_Canceled = 4,
	MobileInputPhase_MAX = 5
};

// Object: Enum MoeGameCore.EMoeActorsFilterType
enum class EMoeActorsFilterType : uint8_t
{
	None = 0,
	FilterClass = 1,
	FilterTag = 2,
	ExcludeTag = 4,
	FilterGameplayTag = 8,
	ExcludeGameplayTag = 16,
	EMoeActorsFilterType_MAX = 17
};

// Object: Enum MoeGameCore.EMoeActorGroupType
enum class EMoeActorGroupType : uint8_t
{
	AllPlayers = 0,
	AllPlaceableActors = 1,
	SelectedActors = 2,
	Self = 3,
	EMoeActorGroupType_MAX = 4
};

// Object: Enum MoeGameCore.EMoeActionTargetType
enum class EMoeActionTargetType : uint8_t
{
	None = 0,
	Self = 1,
	Handler = 2,
	FindInVarMap = 3,
	SelectInLevel = 4,
	EMoeActionTargetType_MAX = 5
};

// Object: Enum MoeGameCore.EActionOverridenFlag
enum class EActionOverridenFlag : uint8_t
{
	NoOverriden = 0,
	DoAction = 1,
	ReverseAction = 2,
	SetVar = 4,
	SetData = 8,
	EActionOverridenFlag_MAX = 9
};

// Object: Enum MoeGameCore.EArchiveAreaApplyType
enum class EArchiveAreaApplyType : uint8_t
{
	EAAT_Personal = 0,
	EAAT_Team = 1,
	EAAT_All = 2,
	EAAT_MAX = 3
};

// Object: Enum MoeGameCore.EAssetsPoolUnloadType
enum class EAssetsPoolUnloadType : uint8_t
{
	UnloadType_Never = 0,
	UnloadType_Delegate = 1,
	UnloadType_Time = 2,
	UnloadType_MAX = 3
};

// Object: Enum MoeGameCore.EAttrType
enum class EAttrType : uint8_t
{
	Invalid = 0,
	Float = 1,
	Bool = 2,
	String = 3,
	Int = 4,
	EAttrType_MAX = 5
};

// Object: Enum MoeGameCore.EOrnamentType
enum class EOrnamentType : uint8_t
{
	None = 0,
	Face = 1,
	Head = 2,
	Back = 3,
	HandWear = 4,
	Free = 5,
	MAX = 6
};

// Object: Enum MoeGameCore.EOrnamentModeType
enum class EOrnamentModeType : uint8_t
{
	None = 0,
	SM = 1,
	SK = 2,
	MAX = 3
};

// Object: Enum MoeGameCore.EWriteFileRet
enum class EWriteFileRet : uint8_t
{
	WFR_Success = 0,
	WFR_Failure = 1,
	WFR_InvalidPath = 2,
	WFR_InvalidContent = 3,
	WFR_FileExist = 4,
	WFR_MAX = 5
};

// Object: Enum MoeGameCore.EVisibleDistanceMode
enum class EVisibleDistanceMode : uint8_t
{
	VDM_Near = 0,
	VDM_Middle = 1,
	VDM_Far = 2,
	VDM_ExtreamlyFar = 3,
	VDM_Default = 4,
	VDM_Num = 5,
	VDM_MAX = 6
};

// Object: Enum MoeGameCore.EMoeBTAIPathMoveState
enum class EMoeBTAIPathMoveState : uint8_t
{
	Invalid = 0,
	Success = 1,
	Running = 2,
	Failure_PathFind = 3,
	Failure_Stuck = 4,
	EMoeBTAIPathMoveState_MAX = 5
};

// Object: Enum MoeGameCore.EMoeBTAIPathFindState
enum class EMoeBTAIPathFindState : uint8_t
{
	None = 0,
	InProgress = 1,
	Success = 2,
	Failure = 3,
	EMoeBTAIPathFindState_MAX = 4
};

// Object: Enum MoeGameCore.EBTAICompareOp
enum class EBTAICompareOp : uint8_t
{
	Invalid = 0,
	EqualTo = 1,
	UnequalTo = 2,
	GreaterThan = 3,
	LessThan = 4,
	GreaterThanOrEqualTo = 5,
	LessThanOrEqualTo = 6,
	EBTAICompareOp_MAX = 7
};

// Object: Enum MoeGameCore.EBTAICheckOp
enum class EBTAICheckOp : uint8_t
{
	Invalid = 0,
	EqualTo = 1,
	UnequalTo = 2,
	EBTAICheckOp_MAX = 3
};

// Object: Enum MoeGameCore.EBTAIConjunctType
enum class EBTAIConjunctType : uint8_t
{
	And = 0,
	Or = 1,
	EBTAIConjunctType_MAX = 2
};

// Object: Enum MoeGameCore.AIBuffEffect
enum class EAIBuffEffect : uint8_t
{
	Buff_Effect_Type_None = 0,
	Buff_Effect_Type_SpeedUp = 1,
	Buff_Effect_Type_SpeedDown = 2,
	Buff_Effect_Type_AddHP = 3,
	Buff_Effect_Type_SubHP = 4,
	Buff_Effect_Type_Freeze = 5,
	Buff_Effect_Type_NoMove = 6,
	Buff_Effect_Type_Vertigo = 7,
	Buff_Effect_Type_Invincible = 8,
	Buff_Effect_Type_Invisible = 9,
	Buff_Effect_Type_Others = 99,
	Buff_Effect_Type_MAX = 100
};

// Object: Enum MoeGameCore.EBuffReplicateType
enum class EBuffReplicateType : uint8_t
{
	None = 0,
	ServerToClient = 1,
	ClientToServer = 2,
	EBuffReplicateType_MAX = 3
};

// Object: Enum MoeGameCore.EBuffRemoveReason
enum class EBuffRemoveReason : uint8_t
{
	TimeOver = 0,
	Rebirth = 1,
	Reset = 2,
	Other = 3,
	EBuffRemoveReason_MAX = 4
};

// Object: Enum MoeGameCore.ECameraMainPlayerStateSwitchInfo
enum class ECameraMainPlayerStateSwitchInfo : uint8_t
{
	OnTaskStart = 0,
	OnLowLevelTaskEnter = 1,
	ECameraMainPlayerStateSwitchInfo_MAX = 2
};

// Object: Enum MoeGameCore.EMoeCameraCollisionTestMode
enum class EMoeCameraCollisionTestMode : uint8_t
{
	Default = 0,
	AsymptoticInterpolationZoomIn = 1,
	SpringDamperZoomIn = 2,
	NoCollisionTest = 3,
	EMoeCameraCollisionTestMode_MAX = 4
};

// Object: Enum MoeGameCore.EMoeCameraDataProcessorPriority
enum class EMoeCameraDataProcessorPriority : uint8_t
{
	Modifier_Processor = 1,
	RotationInput_Processor = 2,
	RotAdapt_Processor = 11,
	SurviveGameLocBias_Processor = 12,
	RaceGameLocBias_Processor = 13,
	ArmLengthWithPitch_Processor = 21,
	LocationLag_Processor = 51,
	SPGameLag_Processor = 52,
	SPRotationModifier_Processor = 54,
	SPSOCPressModifier_Processor = 55,
	SPLockTarget_Processor = 56,
	SPClickSOC_Processor = 57,
	SPPitchReset_Processor = 58,
	SPBattle_Processor = 59,
	SPBattleTurn_Processor = 60,
	SocketZAdjust_Processor = 61,
	SPYawReset_Processor = 62,
	SPInteractTurn_Processor = 63,
	SPTargetAssistCtrl_Processor = 64,
	SPBattleAssistCtrl_Processor = 65,
	SPTargetOffsetProcessor = 66,
	SPRotateToTargetProcessor = 67,
	LockTarget_Processor = 71,
	Follow2D_Processor = 81,
	SPCameraSequence_Processor = 91,
	EMoeCameraDataProcessorPriority_MAX = 92
};

// Object: Enum MoeGameCore.EMoeCameraMainPlayerStatePriority
enum class EMoeCameraMainPlayerStatePriority : uint8_t
{
	Invalid = 0,
	InGame = 1,
	Lobby = 2,
	DDP = 3,
	UGC = 4,
	Home = 5,
	Farm = 6,
	Cook = 7,
	House = 8,
	CustomOverride = 9,
	Spectator = 10,
	MoeGPOOverride = 11,
	HouseEdit = 12,
	FarmCrazy = 13,
	AimShootOverride = 14,
	Follow2D = 15,
	Arena_CustomMode = 16,
	UGCOverride = 17,
	UGC_BlockCodingOverride = 18,
	Arena_CustomOverride = 19,
	Arena_CameraAdjustAbleTask = 20,
	LobbyShuttlePrepare = 21,
	LobbyShuttleFly = 22,
	UnKnown = 23,
	UnKnown = 24,
	UnKnown = 25,
	UnKnown = 26,
	UnKnown = 27,
	SPGame_Default = 28,
	SPGame_BattleDefault = 29,
	SPGame_LevelCtrl1 = 30,
	SPGame_LevelCtrl2 = 31,
	SPGame_LevelCtrl3 = 32,
	SPGame_HoldRangeWeapon = 33,
	SPGame_Swim = 34,
	SPGame_Stroke = 35,
	SPGame_Feed = 36,
	SPGame_Climb = 37,
	SPGame_Riding = 38,
	SPGame_WeaponAim = 39,
	SPGame_FriendSkill = 40,
	SPGame_Aim = 41,
	SPGame_SkillTask = 42,
	SPGame_SkillBossPerform = 43,
	SPGame_Test = 44,
	SPGame_ClickSOC = 45,
	SPGame_PressSOC = 46,
	SPGame_LockTarget = 47,
	SPGame_NPCInteract = 48,
	SPGame_Sequence = 49,
	SPGame_BattleTeleport = 50,
	SPGame_RiddenAim = 51,
	EMoeCameraMainPlayerStatePriority_MAX = 52
};

// Object: Enum MoeGameCore.EMoeCameraModifierMode
enum class EMoeCameraModifierMode : uint8_t
{
	Relative = 0,
	Absolute = 1,
	EMoeCameraModifierMode_MAX = 2
};

// Object: Enum MoeGameCore.EMoeCameraModifierLerp
enum class EMoeCameraModifierLerp : uint8_t
{
	None = 0,
	Lerp = 1,
	Immediate = 2,
	EMoeCameraModifierLerp_MAX = 3
};

// Object: Enum MoeGameCore.EMoeCameraModifierPriority
enum class EMoeCameraModifierPriority : uint8_t
{
	Default = 0,
	Lobby = 1,
	InGame = 2,
	DDP = 3,
	UGC = 4,
	Home = 5,
	Farm = 6,
	Cook = 7,
	House = 8,
	CustomOverride = 9,
	SpectatorBack = 10,
	SpectatorForward = 11,
	SpectatorRight = 12,
	SpectatorTop = 13,
	LevelCustomOverride = 14,
	KillerCamera = 15,
	Arena_CustomMode = 16,
	HouseEdit = 17,
	FarmCrazy = 18,
	Invalid_DefaultFlag = 19,
	UGCOverride = 20,
	Home_Furniture_Camera = 21,
	UGC_BlockCodingOverride = 22,
	Arena_CustomOverride = 23,
	Arena_CameraAdjustAbleTask = 24,
	LobbyShuttlePrepare = 25,
	LobbyShuttleFly = 26,
	Stun_Camera = 27,
	Photo = 28,
	UnKnown = 29,
	UnKnown = 30,
	UnKnown = 31,
	UnKnown = 32,
	UnKnown = 33,
	SPGame_Default = 34,
	SPGame_BattleDefault = 35,
	SPGame_LevelCtrl1 = 36,
	SPGame_LevelCtrl2 = 37,
	SPGame_LevelCtrl3 = 38,
	SPGame_HoldRangeWeapon = 39,
	SPGame_Swim = 40,
	SPGame_Stroke = 41,
	SPGame_Feed = 42,
	SPGame_Climb = 43,
	SPGame_Riding = 44,
	SPGame_WeaponAim = 45,
	SPGame_FriendSkill = 46,
	SPGame_Aim = 47,
	SPGame_SkillTask = 48,
	SPGame_SkillBossPerform = 49,
	SPGame_Test = 50,
	SPGame_ClickSOC = 51,
	SPGame_PressSOC = 52,
	SPGame_LockTarget = 53,
	SPGame_NPCInteract = 54,
	SPGame_BattleTeleport = 55,
	SPGame_RiddenAim = 56,
	EMoeCameraModifierPriority_MAX = 57
};

// Object: Enum MoeGameCore.EMoeCameraParamsType
enum class EMoeCameraParamsType : uint8_t
{
	Invalid = 0,
	MoeCameraBasicParam = 1,
	MoeCameraModifierParam = 2,
	EMoeCameraParamsType_MAX = 3
};

// Object: Enum MoeGameCore.EMoeCameraTickType
enum class EMoeCameraTickType : uint8_t
{
	Invalid = 0,
	Tick_Normal = 1,
	Tick_DDP = 2,
	EMoeCameraTickType_MAX = 3
};

// Object: Enum MoeGameCore.EMoeCameraUGCShootingGameType
enum class EMoeCameraUGCShootingGameType : uint8_t
{
	Moe_UGC_Shooting_None = 0,
	Moe_UGC_Shooting_2D = 1,
	Moe_UGC_Shooting_Depression = 2,
	Moe_UGC_Shooting_MAX = 3
};

// Object: Enum MoeGameCore.EMoeCameraManagerStatePriority
enum class EMoeCameraManagerStatePriority : uint8_t
{
	Moe_MainPlayer_Camera = 0,
	Moe_Community_NPC_Camera = 1,
	Moe_Community_Preview_Camera = 2,
	Moe_Community_Lottery_Camera = 3,
	Moe_Community_GoldSuit_Camera = 4,
	Moe_Community_FaceEmotion_Camera = 5,
	Moe_ShootGame_1P_Camera = 6,
	Moe_ShootGame_3P_Camera = 7,
	Moe_ShootGame_Dead_Camera = 8,
	Moe_Community_GoldSuit_Camera2 = 9,
	Moe_Community_PlayFirework_Camera = 10,
	Moe_SnowboardGame_Camera = 11,
	Moe_Dialogue_Camera = 12,
	Moe_Custom_Camera = 13,
	Moe_DDP_Begin_Camera = 14,
	Moe_Vehicle = 15,
	Moe_MCG_Camera = 16,
	Moe_Community_LevelSequence = 17,
	Moe_MAY_FollowCamera = 18,
	Moe_Simple_Move = 19,
	Moe_NR3E8_Camera = 20,
	Moe_UGC_EGame_Camera = 21,
	Moe_UGC_EGod_Camera = 22,
	Moe_UGC_EPhoto_Camera = 23,
	Moe_UGC_Fixed_Camera = 24,
	Moe_UGC_Static_Camera = 25,
	Moe_UGC_Spline_Camera = 26,
	Moe_UGC_DynamicPrevew_Camera = 27,
	Moe_UGC_AvatarPreview_Camera = 28,
	Moe_UGC_Preview_Camera = 29,
	Moe_Home_TripCamera = 30,
	Moe_UGC_EPhotoInPlay_Camera = 31,
	Moe_Arena_OverLook_Camera = 32,
	Moe_Arena_Traditional_Camera = 33,
	Moe_COC_TopDown_Camera = 34,
	Moe_COC_Simple_Move = 35,
	Moe_COC_Camera_LUA1 = 36,
	Moe_COC_Camera_LUA2 = 37,
	Moe_COC_Camera_LUA3 = 38,
	Moe_COC_Camera_LUA4 = 39,
	Moe_COC_LevelSequence_Camera = 40,
	Moe_Hotfix_Camera1 = 41,
	Moe_Hotfix_Camera2 = 42,
	Moe_Hotfix_Camera3 = 43,
	Moe_Hotfix_Camera4 = 44,
	Moe_SP_Fixed_Camera = 45,
	Moe_SP_PVP_Camera = 46,
	Moe_CameraState_Max = 47,
	Moe_MAX = 48
};

// Object: Enum MoeGameCore.EMoeCameraRotToLeftInputDirSpeedMode
enum class EMoeCameraRotToLeftInputDirSpeedMode : uint8_t
{
	Original = 0,
	DotIthCameraRightDir = 1,
	Constant = 2,
	Speed = 3,
	EMoeCameraRotToLeftInputDirSpeedMode_MAX = 4
};

// Object: Enum MoeGameCore.ECamSpMvMode
enum class ECamSpMvMode : uint8_t
{
	Linear = 0,
	EaseInOut = 1,
	EaseIn = 2,
	EaseOut = 3,
	Curve = 4,
	ECamSpMvMode_MAX = 5
};

// Object: Enum MoeGameCore.EGrabCharacterState
enum class EGrabCharacterState : uint8_t
{
	GrabNone = 0,
	GrabStart = 1,
	GrabLoop = 2,
	GrabEnd = 3,
	EGrabCharacterState_MAX = 4
};

// Object: Enum MoeGameCore.EProjectIdleShowState
enum class EProjectIdleShowState : uint8_t
{
	Normal = 0,
	Idle = 1,
	IdleShow = 2,
	IdleAgain = 3,
	EProjectIdleShowState_MAX = 4
};

// Object: Enum MoeGameCore.EFollowMoveAcceptJoinError
enum class EFollowMoveAcceptJoinError : uint8_t
{
	None = 0,
	Reject = 1,
	InviteeDisappear = 2,
	OverDistance = 3,
	StateError = 4,
	EFollowMoveAcceptJoinError_MAX = 5
};

// Object: Enum MoeGameCore.EFollowMoveTurnType
enum class EFollowMoveTurnType : uint8_t
{
	Center = 0,
	Left = 1,
	Right = 2,
	EFollowMoveTurnType_MAX = 3
};

// Object: Enum MoeGameCore.EFollowMoveType
enum class EFollowMoveType : uint8_t
{
	EFollow_TrainMove = 0,
	EFollow_ChineseDragon = 1,
	EFollow_GroupDance = 2,
	EFollow_HandInHand = 3,
	EFollow_Skateboard = 4,
	EFollow_MAX = 5
};

// Object: Enum MoeGameCore.ETrainType
enum class ETrainType : uint8_t
{
	ETrain_Head = 0,
	ETrain_Middle = 1,
	ETrain_End = 2,
	ETrain_MAX = 3
};

// Object: Enum MoeGameCore.EGrabCountLimit
enum class EGrabCountLimit : uint8_t
{
	NoLimit = 0,
	LimitOne = 1,
	EGrabCountLimit_MAX = 2
};

// Object: Enum MoeGameCore.EGrabSwitchType
enum class EGrabSwitchType : uint8_t
{
	None = 0,
	GrabOnly = 1,
	FollowOnly = 2,
	BothGrabAndFollow = 3,
	EGrabSwitchType_MAX = 4
};

// Object: Enum MoeGameCore.EHandHoldBlendType
enum class EHandHoldBlendType : uint8_t
{
	BlendNone = 0,
	BlendIdle = 1,
	BlendMove = 2,
	BlendJump = 3,
	BlendIdleShow = 4,
	BlendExtra1 = 5,
	BlendExtra2 = 6,
	EHandHoldBlendType_MAX = 7
};

// Object: Enum MoeGameCore.EMoeInputCommandType
enum class EMoeInputCommandType : uint8_t
{
	Jump = 0,
	Dive = 1,
	Slide = 2,
	EMoeInputCommandType_MAX = 3
};

// Object: Enum MoeGameCore.EPAStateMachineType
enum class EPAStateMachineType : uint8_t
{
	PA_Motion = 0,
	PA_Action = 1,
	PA_MAX = 2
};

// Object: Enum MoeGameCore.EBeGrabedState
enum class EBeGrabedState : uint8_t
{
	BeGrabNone = 0,
	BeGrabStart = 1,
	BeGrabLoop = 2,
	BeGrabEnd = 3,
	EBeGrabedState_MAX = 4
};

// Object: Enum MoeGameCore.EMoeFollowState
enum class EMoeFollowState : uint8_t
{
	None = 0,
	Start = 1,
	Loop = 2,
	End = 3,
	EMoeFollowState_MAX = 4
};

// Object: Enum MoeGameCore.EPropRemoveReason
enum class EPropRemoveReason : uint16_t
{
	Default = 0,
	ByOtherPlayer = 1,
	Used = 2,
	OverriddenByProp = 3,
	Invalid = 255,
	EPropRemoveReason_MAX = 256
};

// Object: Enum MoeGameCore.EShuttleState
enum class EShuttleState : uint8_t
{
	None = 0,
	Prepare = 1,
	PrepareFinish = 2,
	Launch = 3,
	Down = 4,
	Finish = 5,
	EShuttleState_MAX = 6
};

// Object: Enum MoeGameCore.EMoeSMBoxType
enum class EMoeSMBoxType : uint8_t
{
	ESAFETY_VELOCITY = 0,
	ESAFETY_GRAVITY = 1,
	ESAFETY_HIT = 2,
	ESAFETY_Collision = 3,
	ESAFETY_State = 4,
	ESAFETY_MAX = 5
};

// Object: Enum MoeGameCore.EMoeSMBoxMask
enum class EMoeSMBoxMask : uint8_t
{
	No_Monitor = 0,
	NormalMove = 1,
	StateDive = 2,
	Teleport = 3,
	FrameDada = 4,
	Speed_Monitor = 5,
	All_Monitor = 6,
	ALL_MAX = 7,
	EMoeSMBoxMask_MAX = 8
};

// Object: Enum MoeGameCore.ECharSoundPoint
enum class ECharSoundPoint : uint8_t
{
	None = 0,
	Rebirth = 3,
	Dive = 4,
	ECharSoundPoint_MAX = 5
};

// Object: Enum MoeGameCore.ECharStateMachineType
enum class ECharStateMachineType : uint8_t
{
	ActionStateMachine = 0,
	MotionStateMachine = 1,
	AnimStateMachine = 2,
	PostProcessStateMachine = 3,
	StageStateMachine = 4,
	ECharStateMachineType_MAX = 5
};

// Object: Enum MoeGameCore.EMoeCharacterViewType
enum class EMoeCharacterViewType : uint8_t
{
	FPP = 0,
	TPP = 1,
	EMoeCharacterViewType_MAX = 2
};

// Object: Enum MoeGameCore.ELocalSplinePointType
enum class ELocalSplinePointType : uint8_t
{
	Linear = 0,
	Curve = 1,
	Constant = 2,
	CurveClamped = 3,
	CurveCustomTangent = 4,
	ELocalSplinePointType_MAX = 5
};

// Object: Enum MoeGameCore.EColorChangerAction
enum class EColorChangerAction : uint8_t
{
	NoAction = 0,
	WaitChangeColor = 1,
	ChangeColor = 2,
	RecoverColor = 3,
	EColorChangerAction_MAX = 4
};

// Object: Enum MoeGameCore.EColorChangerState
enum class EColorChangerState : uint8_t
{
	Deactivate = 0,
	ColorChanging = 1,
	ColorRecovering = 2,
	EColorChangerState_MAX = 3
};

// Object: Enum MoeGameCore.EMoeCommunityFacilityID
enum class EMoeCommunityFacilityID : uint8_t
{
	None = 0,
	HotBalloon = 1,
	Corsair = 2,
	Piano = 3,
	DoublePlayerChair = 4,
	SeeSaw = 5,
	BalanceBall = 6,
	TeleportStar = 7,
	Carousel = 8,
	WaterBall = 9,
	BeachChair = 10,
	Pray = 11,
	SwimmingPool = 12,
	WaterSlide = 13,
	EasterEggSwing = 14,
	EasterEggDuckRing = 15,
	EasterEggStar = 16,
	SwingSeat = 17,
	SkyLantern = 21,
	Dragon = 22,
	EMoeCommunityFacilityID_MAX = 23
};

// Object: Enum MoeGameCore.EMoeConnectTaskType
enum class EMoeConnectTaskType : uint8_t
{
	None = 0,
	ReplicateWaitTask = 1,
	ConditionTask = 2,
	EMoeConnectTaskType_MAX = 3
};

// Object: Enum MoeGameCore.ECurveMoveType
enum class ECurveMoveType : uint8_t
{
	SinCurve = 0,
	Line = 1,
	ECurveMoveType_MAX = 2
};

// Object: Enum MoeGameCore.EMoeDsMode
enum class EMoeDsMode : uint8_t
{
	NormalDS = 0,
	Standalone = 1,
	LocalDS = 2,
	DirectDS = 3,
	TrainAI = 4,
	EMoeDsMode_MAX = 5
};

// Object: Enum MoeGameCore.EnumDsCommandType
enum class EnumDsCommandType : uint8_t
{
	Client2DS = 0,
	Client2Client = 1,
	Client2ClientAndDS = 2,
	DS2AllClients = 3,
	DS2DS = 4,
	DS2DSAndAllClients = 5,
	EnumDsCommandType_MAX = 6
};

// Object: Enum MoeGameCore.EMoeEffectActiveState
enum class EMoeEffectActiveState : uint8_t
{
	Invalid = 0,
	Open = 1,
	Close = 2,
	EMoeEffectActiveState_MAX = 3
};

// Object: Enum MoeGameCore.EMoeEffectTypeID
enum class EMoeEffectTypeID : uint8_t
{
	Invalid = 0,
	AddImpulse = 1,
	AddExplodeImpulse = 2,
	AddForce = 3,
	AddRadialForce = 4,
	AddOffset = 5,
	AddDynamicForce = 6,
	AddCrossForce = 7,
	AddInputVector = 8,
	CloseCompCollision = 9,
	OpenCompCollision = 10,
	CloseCollision = 11,
	OpenCollision = 12,
	ChangeStaticMesh = 13,
	ChangeMaterial = 14,
	SetOverlayMaterial = 15,
	ChangeTextureParam = 16,
	ChangeState = 17,
	CloseVisibility = 18,
	OpenVisibility = 19,
	DestroyActor = 20,
	StartMove = 21,
	PlayEmitter = 22,
	StandardHit = 23,
	NormalImpulse = 24,
	VelBiasImpulse = 25,
	VelStaminaBiasImpulse = 26,
	ChangeFloatAttr = 27,
	TickChangeFloatAttr = 28,
	CurveChangeFloatAttr = 29,
	ChangeBoolAttr = 30,
	ChangeIntAttr = 31,
	TeleportWithLocation = 32,
	TeleportWithTransform = 33,
	KeepChangeMaterial = 34,
	PlaySound = 35,
	StopSound = 36,
	DisableEmitter = 37,
	RemoveBuffByAttr = 38,
	RemoveBuffByObject = 39,
	RemoveBuffByTag = 40,
	Sample = 41,
	ChangeMotionState = 42,
	ClearRebound = 43,
	ClearCharacterRebound = 44,
	AddRebound = 45,
	AddReboundToCHaracter = 46,
	WipeVelocity = 47,
	AttachToTarget = 48,
	AddBuffByClass = 49,
	Teleport = 50,
	PostAkEvent = 51,
	AddRadialImpulseToComponent = 52,
	AddRadialForceToComponent = 53,
	TickAddForce = 54,
	PoleVaultAddImpulse = 55,
	CurveAddForce = 56,
	AddExactOffset = 57,
	ChangeCharMaterialScalarParam = 58,
	ChangeCharMaterialVectorParam = 59,
	ChangeMaterialScalarParam = 60,
	ChangeMaterialVectorParam = 61,
	ChangeBulletMaxCount = 62,
	Frozen = 63,
	TryToStun = 64,
	PlayMontage = 65,
	AddTags = 66,
	TakeDamage = 67,
	DamageTip = 68,
	UGC_PlayParticle = 69,
	UGC_PlayAnimation = 70,
	UGC_PlaySound = 71,
	UGC_StopSound = 72,
	UGC_CheckTargets = 73,
	UGC_SingleDamage = 74,
	UGC_Dot = 75,
	UGC_AddBuff = 76,
	UGC_ModifyAttr = 77,
	UGC_Launch = 78,
	UGC_LaunchBullet = 79,
	UGC_Bounce = 80,
	UGC_Attract = 81,
	UGC_CreateEffect = 82,
	UGC_Scale = 83,
	UGC_Invisible = 84,
	UGC_SetInvisible = 85,
	UGC_EndInvisible = 86,
	UGC_PlayProjectile = 87,
	UGC_CameraShake = 88,
	UGC_PureLuaCustom = 89,
	UGC_LaunchProjectile = 90,
	UGC_TeleportToProjectile = 91,
	UGC_SendEvent = 92,
	UGC_AddProp = 93,
	UGC_SetAvatar = 94,
	UGC_Transformation = 95,
	UGC_AddForce = 96,
	UGC_AddRadialForce = 97,
	UGC_SkillSwitch = 98,
	UGC_Impulse = 99,
	NR3E1_Magnet = 100,
	EMoeEffectTypeID_MAX = 101
};

// Object: Enum MoeGameCore.EMoeLogVerbosity
enum class EMoeLogVerbosity : uint8_t
{
	Error = 0,
	Warning = 1,
	Important = 2,
	Normal = 3,
	VerBose = 4,
	EMoeLogVerbosity_MAX = 5
};

// Object: Enum MoeGameCore.EMoeCustomMovementMode
enum class EMoeCustomMovementMode : uint8_t
{
	None = 0,
	Bubbles = 1,
	EMoeCustomMovementMode_MAX = 2
};

// Object: Enum MoeGameCore.EMoeCalculateRangeImpulseType
enum class EMoeCalculateRangeImpulseType : uint8_t
{
	Location = 0,
	LineTrace = 1,
	EMoeCalculateRangeImpulseType_MAX = 2
};

// Object: Enum MoeGameCore.EMoeBombOverlapShape
enum class EMoeBombOverlapShape : uint8_t
{
	Box = 0,
	Sphere = 1,
	EMoeBombOverlapShape_MAX = 2
};

// Object: Enum MoeGameCore.EMoePhysicsProfileType
enum class EMoePhysicsProfileType : uint8_t
{
	None = 0,
	Store = 1,
	ChestProfilePerform = 2,
	ChestProfile = 3,
	NoPA2 = 4,
	EMoePhysicsProfileType_MAX = 5
};

// Object: Enum MoeGameCore.ERoguelikeLevelTaskTypeQQMusic
enum class ERoguelikeLevelTaskTypeQQMusic : uint8_t
{
	E_Unkown = 0,
	E_Elite = 1,
	E_Survival = 2,
	E_Guard = 3,
	E_Escort = 4,
	E_Occupy = 5,
	E_Shop = 6,
	E_Boss = 7,
	E_Annihilate = 8,
	E_Prepare = 9,
	E_Mining = 10,
	E_Chest = 11,
	E_MAX = 12
};

// Object: Enum MoeGameCore.ECharStateType
enum class ECharStateType : uint8_t
{
	None = 0,
	Poison = 1,
	HP = 2,
	Recover = 3,
	Aim = 4,
	ECharStateType_MAX = 5
};

// Object: Enum MoeGameCore.EGameNotifyEvent
enum class EGameNotifyEvent : uint8_t
{
	ArriveTerminal = 0,
	CollideWithVenom = 1,
	CollideWithPlayer = 2,
	ChangePlayerBuff = 3,
	SetEvilPlayer = 4,
	ResetPlayersComplete = 5,
	CrownGameStart = 6,
	CrownGameEnd = 7,
	GetCrown = 8,
	LoadLevelComplete = 9,
	SendBuffEffect = 10,
	SetIsPlayerCatchingPenguin = 11,
	BrokenBucket = 12,
	AIStateUpdate = 13,
	DeadNeedDelayRebirth = 14,
	OnEnterRebirth = 15,
	ApplyArchiveArea = 16,
	EGameNotifyEvent_MAX = 17
};

// Object: Enum MoeGameCore.EMoeGameType
enum class EMoeGameType : uint8_t
{
	EcaRoundGame_EditorMode = 0,
	SimpleGame = 1,
	NewComerGame = 2,
	EcaArchiveGame = 3,
	DisplayGame = 4,
	CommunityGame = 5,
	OGCGame = 6,
	RunningGame = 7,
	SurvivalGame = 8,
	DollsRunGame = 9,
	HideAndSeekGame = 10,
	WerewolfGame = 11,
	DDPGame = 12,
	GunGameGame = 13,
	BioChaseGame = 14,
	RoguelikeGame = 15,
	DDPTutorial = 16,
	GunGameTDMGame = 17,
	TYCGame = 18,
	BrGame = 19,
	GunGameBPMGame = 20,
	TDGame = 21,
	TDSmashGame = 22,
	OMDGame = 23,
	GunGameKCGame = 24,
	DfGame = 25,
	OMDGameArchive = 26,
	UGCRoundGame = 27,
	GameTypePreset02 = 28,
	GameTypePreset03 = 29,
	GameTypePreset04 = 30,
	GameTypePreset05 = 31,
	GameTypePreset06 = 32,
	GameTypePreset07 = 33,
	GameTypePreset08 = 34,
	GameTypePreset09 = 35,
	JSGame = 36,
	JSGameBegin = 37,
	JSBikeGame = 38,
	JSKartGame = 39,
	JSGameEnd = 40,
	NR3E8RichGame = 41,
	MoeChaseGame = 42,
	MAYGame = 43,
	UGCEditorGame = 44,
	UGCPlayGame = 45,
	HomeGame = 46,
	FarmGame = 47,
	CookGame = 48,
	HouseGame = 49,
	FarmCrazyGame = 50,
	ProjectTGame = 51,
	DDPGameModeBegin = 52,
	DDPTeamCompetitionGame = 53,
	DDPGameModeEnd = 54,
	DDBGame = 55,
	DNDGame = 56,
	ArenaGame = 57,
	ArenaSelectCharacterGame = 58,
	ArenaMainGame = 59,
	ArenaSubGame = 60,
	ArenaPrepareCombatSubGame = 61,
	ArenaNormalCombatSubGame = 62,
	ArenaFinalCombatSubGame = 63,
	ArenaDrawCardShowingSubGame = 64,
	ArenaFinalPrepareSubGame = 65,
	ArenaPreDrawCardShowingSubGame = 66,
	BSGame = 67,
	BSGuideGame = 68,
	MobaFootBallGame = 69,
	MobaFootBallGuideGame = 70,
	HotZoneGame = 71,
	HotZoneMainGame = 72,
	HotZoneSelectCharacterGame = 73,
	HotZoneNormalCombatSubGame = 74,
	HotZonePrepareCombatSubGame = 75,
	HotZoneGuideGame = 76,
	HotZoneGuideMainGame = 77,
	HotZoneGuideSelectCharacterGame = 78,
	HotZoneGuideNormalCombatSubGame = 79,
	HotZoneGuidePrepareCombatSubGame = 80,
	ArenaTutorialGame = 81,
	ArenaTutorialSelectCharacterGame = 82,
	ArenaTutorialMainGame = 83,
	ArenaTutorialSubGame = 84,
	ArenaTutorialPrepareCombatSubGame = 85,
	ArenaTutorialNormalCombatSubGame = 86,
	ArenaTutorialFinalCombatSubGame = 87,
	ArenaTutorialDrawCardShowingSubGame = 88,
	ArenaTutorialFinalPrepareSubGame = 89,
	ArenaTutorialPreDrawCardShowingSubGame = 90,
	HOKHeistGame = 91,
	HOKMechaGame = 92,
	HOKGameArchive = 93,
	HOKGame = 94,
	HOKSelectRoleGame = 95,
	HOKClashGameArchive = 96,
	HOKClashGame = 97,
	HOKClashSelectRoleGame = 98,
	ArenaGuideGame = 99,
	ArenaGuideSelectCharacterGame = 100,
	ArenaGuideMainGame = 101,
	ArenaGuideSubGame = 102,
	ArenaGuidePrepareCombatSubGame = 103,
	ArenaGuideNormalCombatSubGame = 104,
	ArenaGuideFinalCombatSubGame = 105,
	ArenaGuideDrawCardShowingSubGame = 106,
	ArenaGuideFinalPrepareSubGame = 107,
	ArenaGuidePreDrawCardShowingSubGame = 108,
	ArenaTrainingGame = 109,
	HOKTrainingGameArchive = 110,
	HOKTrainingGame = 111,
	HOKTrainingSelectRoleGame = 112,
	LobbyGame = 113,
	UGCLobbyGame = 114,
	LobbyUGCMainCharacterGame = 115,
	LobbyUGCArchiveGame = 116,
	LobbyUGCGameTypeMax = 117,
	LobbyUGCGameTypeArchiveGameStart = 118,
	LobbyUGCGameTypeArchiveGameEnd = 119,
	LobbyUGCGameTypeMin = 120,
	AITrainLoopGame = 121,
	EMoeGameType_MAX = 122
};

// Object: Enum MoeGameCore.EAudioAttenuationType
enum class EAudioAttenuationType : uint8_t
{
	Linear = 0,
	BrickWall = 1,
	CloseDrop = 2,
	FarDrop = 3,
	EAudioAttenuationType_MAX = 4
};

// Object: Enum MoeGameCore.EAppearanceSwitchType
enum class EAppearanceSwitchType : uint8_t
{
	InstantSwitch = 0,
	LerpTransitionSwitch = 1,
	EAppearanceSwitchType_MAX = 2
};

// Object: Enum MoeGameCore.EAudioType
enum class EAudioType : uint8_t
{
	SfxAudio = 0,
	UIAudio = 1,
	LongSfxAudio = 2,
	UGCAudio = 3,
	EAudioType_MAX = 4
};

// Object: Enum MoeGameCore.EParticleInheritOwnerProperty
enum class EParticleInheritOwnerProperty : uint32_t
{
	PIOP_Scale = 1,
	PIOP_Rotation = 2,
	PIOP_Translation = 4,
	PIOP_All = 65535,
	PIOP_MAX = 65536
};

// Object: Enum MoeGameCore.EParticleAttachPoint
enum class EParticleAttachPoint : uint8_t
{
	BottomCenter = 0,
	Center = 1,
	TopCenter = 2,
	EParticleAttachPoint_MAX = 3
};

// Object: Enum MoeGameCore.EGameplayCueType
enum class EGameplayCueType : uint8_t
{
	Appearance = 0,
	Particle = 1,
	Sound = 2,
	Animation = 3,
	Cue_Max = 4,
	EGameplayCueType_MAX = 5
};

// Object: Enum MoeGameCore.EReplayGoToCheckPointResult
enum class EReplayGoToCheckPointResult : uint8_t
{
	Success = 0,
	InvalidCheckPoint = 1,
	NoCheckPoint = 2,
	EReplayGoToCheckPointResult_MAX = 3
};

// Object: Enum MoeGameCore.ELevelType
enum class ELevelType : uint16_t
{
	Archive = 0,
	Racing = 1,
	Survival = 2,
	SinglePoint = 3,
	GroupPoint = 4,
	Camp = 5,
	UGCEditorGame = 6,
	UGCPlayGame = 7,
	TrailPlayGame = 8,
	UCommunityGame = 9,
	DDP = 10,
	TeamChampionRacing = 11,
	SurvivalRacing = 12,
	BnbGame = 16,
	FPS_GunGame_SinglePoint = 1001,
	FPS_GunGame_GroupPoint = 1002,
	FPS_GunGame_BioChase = 1003,
	RoguelikeGame = 1004,
	BrGame = 1005,
	DfGame = 1006,
	ELevelType_MAX = 1007
};

// Object: Enum MoeGameCore.EMoeMatchType
enum class EMoeMatchType : uint8_t
{
	None = 0,
	Relax = 1,
	RelaxDoublePlayer = 2,
	RelaxFourPlayer = 3,
	Rank = 4,
	RankDoublePlayer = 5,
	RankFourPlayer = 6,
	WhoIsTheKiller = 7,
	HideAndSeek = 8,
	E3Test = 9,
	FpsSingle = 10,
	FpsDoublePlayer = 11,
	FpsFourPlayer = 12,
	UGCEditor = 13,
	UGCPlay = 14,
	UGCTour = 15,
	UGCTourV2 = 16,
	UGCMultiPlay = 17,
	UGCMultiTrial = 18,
	UGCUgcCoPlayerRoom = 19,
	MoeChaseGame = 20,
	MoeChaseGame2V8 = 21,
	MoeChaseRankedMode = 22,
	MoeChaseGame3V9 = 23,
	MAYGAME = 24,
	DDP = 25,
	TimeLimitGameTest = 26,
	TimeLimitGamePreviewTest = 27,
	ForestThemeSeason = 28,
	WeaponMaster = 29,
	BiochemicalChase = 30,
	WeaponMaster2 = 31,
	BioFight = 32,
	TeamDeathMatch = 33,
	BattleRoyale1 = 34,
	BattleRoyale2 = 35,
	BattleRoyale4 = 36,
	Roguelike1 = 37,
	Roguelike2 = 38,
	Roguelike4 = 39,
	DF1 = 40,
	DF2 = 41,
	DF4 = 42,
	GunGameKC = 43,
	JSSingle = 44,
	JSBegin = 45,
	JSDouble = 46,
	JSFoursome = 47,
	JSKartSingle = 48,
	JSKartDouble = 49,
	JSKartFoursome = 50,
	JSRKKartSingle = 51,
	JSRKKartDouble = 52,
	JSRKKartFoursome = 53,
	JSRKSingle = 54,
	JSRKDouble = 55,
	JSRKFoursome = 56,
	JSEnd = 57,
	TFGame = 58,
	TDGame = 59,
	TDSGame = 60,
	NR3E8RichBegin = 61,
	NR3E8RichEnd = 62,
	SPGame = 63,
	QATestSingle = 64,
	QATestDoublePlayer = 65,
	QATestFourPlayer = 66,
	UGCMatch = 67,
	UGCMatchEnd = 68,
	UGCUgcCoPlay = 69,
	EMoeMatchType_MAX = 70
};

// Object: Enum MoeGameCore.EMoeGroupActorCollisionType
enum class EMoeGroupActorCollisionType : uint8_t
{
	None = 0,
	Complex = 1,
	Simple = 2,
	EMoeGroupActorCollisionType_MAX = 3
};

// Object: Enum MoeGameCore.EPyhsicsHitType
enum class EPyhsicsHitType : uint8_t
{
	None = 0,
	Character_Body = 1,
	Light_Dynamics = 2,
	Strong_Dynamics = 3,
	Ice_Hit = 4,
	Electricity = 5,
	Scene_Item = 6,
	Posion = 7,
	DDP_Electricity = 8,
	DDP_Ice = 9,
	DDP_FallToDeath = 10,
	Strong_ByMonster = 11,
	DDB_DodgeBall = 12,
	EPyhsicsHitType_MAX = 13
};

// Object: Enum MoeGameCore.EDefaultDirection
enum class EDefaultDirection : uint8_t
{
	CameraForward = 0,
	CameraBack = 1,
	VelocityForward = 2,
	VelocityBack = 3,
	NearestEnemy = 4,
	EDefaultDirection_MAX = 5
};

// Object: Enum MoeGameCore.EMoeInputBufferRemoveReason
enum class EMoeInputBufferRemoveReason : uint8_t
{
	Manual = 0,
	AutoClean = 1,
	EMoeInputBufferRemoveReason_MAX = 2
};

// Object: Enum MoeGameCore.EMoeInputBufferState
enum class EMoeInputBufferState : uint8_t
{
	Pending = 0,
	Executing = 1,
	Finished = 2,
	Canceled = 3,
	Expired = 4,
	EMoeInputBufferState_MAX = 5
};

// Object: Enum MoeGameCore.EMoeInteractionError
enum class EMoeInteractionError : uint16_t
{
	NoError = 0,
	InValidPlayer = 10000,
	PlayerAllreadyStartInteraction = 10001,
	PlayerNotInRange = 10002,
	CharMotionStateNotMatch = 10003,
	CharActionStateNotMatch = 10004,
	CharInteractionStateNotMatch = 10005,
	InValidObjectId = 20000,
	InValidObject = 20001,
	ObjectIsOccupied = 20002,
	InValidInteractionId = 30001,
	EMoeInteractionError_MAX = 30002
};

// Object: Enum MoeGameCore.EMoeIaBaseBpFlag
enum class EMoeIaBaseBpFlag : uint8_t
{
	GetPriority = 0,
	CanOperate = 1,
	OnOperate = 2,
	GetUMGImage = 3,
	GetIsShowOtherUMGButton = 4,
	OnClickButton = 5,
	OnCharacterStateChange = 6,
	IsNeedUpdateCharacter = 7,
	UpdateCharacter = 8,
	OnEnterOverlap = 9,
	OnLeaveOverlap = 10,
	OnOperationRes = 11,
	ClearInvalidData = 12,
	OnCharacterReset = 13,
	EMoeIaBaseBpFlag_MAX = 14
};

// Object: Enum MoeGameCore.EMoeLevelLoadType
enum class EMoeLevelLoadType : uint8_t
{
	Default = 0,
	StreamingLevel = 1,
	DynamicStreamingLevel = 2,
	PbinStreamingLevel = 3,
	EMoeLevelLoadType_MAX = 4
};

// Object: Enum MoeGameCore.ERuleTriggerType
enum class ERuleTriggerType : uint8_t
{
	Unknown = 0,
	Speed = 1,
	OnceLifeSpeed = 2,
	Survival = 3,
	ERuleTriggerType_MAX = 4
};

// Object: Enum MoeGameCore.EMstSubMoveState
enum class EMstSubMoveState : uint8_t
{
	Default = 0,
	BeChasing = 1,
	AdjustMove = 2,
	EMstSubMoveState_MAX = 3
};

// Object: Enum MoeGameCore.EMonsterAttackType
enum class EMonsterAttackType : uint8_t
{
	MAT_Head = 0,
	MAT_FullBody = 1,
	MAT_MeleeWeaponHeavy = 2,
	MAT_ForSpecial = 3,
	Max = 4
};

// Object: Enum MoeGameCore.EMonsterAIInitialState
enum class EMonsterAIInitialState : uint8_t
{
	Idle = 0,
	Patrol = 1,
	EMonsterAIInitialState_MAX = 2
};

// Object: Enum MoeGameCore.EAttributeType
enum class EAttributeType : uint8_t
{
	EAT_Float = 0,
	EAT_Int = 1,
	EAT_Bool = 2,
	EAT_String = 3,
	EAT_MAX = 4
};

// Object: Enum MoeGameCore.ERuntimeCalcMotion
enum class ERuntimeCalcMotion : uint8_t
{
	RCM_None = 0,
	RuntimeRotation = 1,
	RuntimeLocation = 2,
	ERuntimeCalcMotion_MAX = 3
};

// Object: Enum MoeGameCore.EMotionState
enum class EMotionState : uint8_t
{
	MS_InValid = 0,
	MS_UnStart = 1,
	MS_Moving = 2,
	MS_Reversing = 3,
	MS_Sleep = 4,
	MS_MAX = 5
};

// Object: Enum MoeGameCore.EMultiplayerInteractionType
enum class EMultiplayerInteractionType : uint8_t
{
	Default = 0,
	SkippingRope = 1,
	Max = 2
};

// Object: Enum MoeGameCore.ECollisionTriggerType
enum class ECollisionTriggerType : uint8_t
{
	ECT_Hit = 0,
	ECT_Overlap = 1,
	ECT_MAX = 255
};

// Object: Enum MoeGameCore.EOgcMonsterBBKeyType
enum class EOgcMonsterBBKeyType : uint8_t
{
	EBT_Int = 0,
	EBT_Float = 1,
	EBT_MAX = 255
};

// Object: Enum MoeGameCore.EOrnamentAssetType
enum class EOrnamentAssetType : uint8_t
{
	None = 0,
	SM = 1,
	SK = 2,
	EOrnamentAssetType_MAX = 3
};

// Object: Enum MoeGameCore.EMoeMainGameCharacterSoftMode
enum class EMoeMainGameCharacterSoftMode : uint8_t
{
	Soft = 1,
	Comfort = 2,
	Stable = 3,
	EMoeMainGameCharacterSoftMode_MAX = 4
};

// Object: Enum MoeGameCore.EMovementRepChannel
enum class EMovementRepChannel : uint8_t
{
	Rep_Never = 0,
	Rep_Location_XY = 1,
	Rep_Location_Z = 2,
	Rep_Rotation_XY = 4,
	Rep_Rotation_Z = 8,
	Rep_MAX = 9
};

// Object: Enum MoeGameCore.EMoePlaceableStaticIdentifyResult
enum class EMoePlaceableStaticIdentifyResult : uint8_t
{
	None = 0,
	Batchable = 1,
	UnBatchable = 2,
	EMoePlaceableStaticIdentifyResult_MAX = 3
};

// Object: Enum MoeGameCore.EMoePlaceableStaticSetting
enum class EMoePlaceableStaticSetting : uint8_t
{
	None = 0,
	ForceNotStatic = 1,
	ForceStatic = 2,
	Auto = 3,
	EMoePlaceableStaticSetting_MAX = 4
};

// Object: Enum MoeGameCore.EObjectPoolState
enum class EObjectPoolState : uint8_t
{
	None = 0,
	Discard = 1,
	Using = 2,
	EObjectPoolState_MAX = 3
};

// Object: Enum MoeGameCore.EUGCPhysicsSettingMassType
enum class EUGCPhysicsSettingMassType : uint8_t
{
	Mass = 1,
	Density = 2,
	EUGCPhysicsSettingMassType_MAX = 3
};

// Object: Enum MoeGameCore.EPlaceableRuntimeCollision
enum class EPlaceableRuntimeCollision : uint8_t
{
	Default = 0,
	OpenCollision = 1,
	CloseCollision = 2,
	EPlaceableRuntimeCollision_MAX = 3
};

// Object: Enum MoeGameCore.EPlaceableCollisionType
enum class EPlaceableCollisionType : uint8_t
{
	None = 0,
	Complex = 1,
	Simple = 2,
	EPlaceableCollisionType_MAX = 3
};

// Object: Enum MoeGameCore.EMoePlaceableActorRunningState
enum class EMoePlaceableActorRunningState : uint8_t
{
	Default = 0,
	Place = 1,
	MotionTest = 2,
	Active = 3,
	Disable = 4,
	Removed = 5,
	EMoePlaceableActorRunningState_MAX = 6
};

// Object: Enum MoeGameCore.EDirection
enum class EDirection : uint8_t
{
	Direction_None = 0,
	Direction_Front = 1,
	Direction_Back = 2,
	Direction_Left = 3,
	Direction_Right = 4,
	Direction_FL = 5,
	Direction_FR = 6,
	Direction_BL = 7,
	Direction_BR = 8,
	Direction_MAX = 9
};

// Object: Enum MoeGameCore.EMoePlayerDisableFlags
enum class EMoePlayerDisableFlags : uint8_t
{
	Normal = 0,
	DisableMovement = 1,
	DisableInput = 2,
	DisableTrigger = 4,
	DisableVisibility = 8,
	DisableActorTick = 16,
	DisableMeshComponentTick = 32,
	EMoePlayerDisableFlags_MAX = 33
};

// Object: Enum MoeGameCore.EPropType
enum class EPropType : uint8_t
{
	Default = 0,
	GameplayAbility = 1,
	Max = 255
};

// Object: Enum MoeGameCore.EMoePropDamageType
enum class EMoePropDamageType : uint8_t
{
	Gun = 0,
	Melee = 1,
	Explosion = 2,
	EMoePropDamageType_MAX = 3
};

// Object: Enum MoeGameCore.EMoePropRemoveTypeOnDead
enum class EMoePropRemoveTypeOnDead : uint8_t
{
	Destroy = 0,
	BecomeNotPick = 1,
	Keep = 2,
	EMoePropRemoveTypeOnDead_MAX = 3
};

// Object: Enum MoeGameCore.EMoePropSlotOperateType
enum class EMoePropSlotOperateType : uint8_t
{
	Touch = 0,
	Click = 1,
	EMoePropSlotOperateType_MAX = 2
};

// Object: Enum MoeGameCore.EMoePropSlotType
enum class EMoePropSlotType : uint8_t
{
	Default = 0,
	Hide = 1,
	EMoePropSlotType_MAX = 2
};

// Object: Enum MoeGameCore.EMoePropUseStateEndType
enum class EMoePropUseStateEndType : uint8_t
{
	PUSE_None = 0,
	PUSE_Timer = 1,
	PUSE_Interrupt = 2,
	PUSE_MAX = 3
};

// Object: Enum MoeGameCore.EMoePropShootGunType
enum class EMoePropShootGunType : uint8_t
{
	None = 0,
	Stain = 1,
	Artillery = 2,
	Firework = 3,
	VacuumCleaner = 4,
	TankBullet = 5,
	TankLaser = 6,
	EMoePropShootGunType_MAX = 7
};

// Object: Enum MoeGameCore.EMoePropStateEndType
enum class EMoePropStateEndType : uint8_t
{
	Authority = 1,
	Autonomous = 2,
	Simulated = 4,
	EMoePropStateEndType_MAX = 5
};

// Object: Enum MoeGameCore.EMoePropEnergyAddType
enum class EMoePropEnergyAddType : uint8_t
{
	PropHit = 0,
	Kill = 1,
	Other = 2,
	EMoePropEnergyAddType_MAX = 3
};

// Object: Enum MoeGameCore.EAbilityType
enum class EAbilityType : uint8_t
{
	GameplayAbility = 0,
	Prop = 1,
	Max = 255
};

// Object: Enum MoeGameCore.EMoePropShootState
enum class EMoePropShootState : uint8_t
{
	Prepare = 0,
	Start = 1,
	Loop = 2,
	EMoePropShootState_MAX = 3
};

// Object: Enum MoeGameCore.EClassRepNodeMapping
enum class EClassRepNodeMapping : uint8_t
{
	NotRouted = 0,
	RelevantAllConnections = 1,
	IsNetRelevantFuncDependent = 2,
	Spatialize_Static = 3,
	Spatialize_Dynamic = 4,
	Spatialize_Dormancy = 5,
	Moba_CharacterFrequencyLimiter = 6,
	Moba_NetDistanceCut = 7,
	Mayday_CustomNetCut = 8,
	Moba_NetUpdateTime = 9,
	EClassRepNodeMapping_MAX = 10
};

// Object: Enum MoeGameCore.EMoeValidationActorType
enum class EMoeValidationActorType : uint8_t
{
	EValidationActor_FanBase = 1,
	EValidationActor_SwingBallBase = 2,
	EValidationActor_RotateBarBase = 3,
	EValidationActor_BumpersBase = 4,
	EValidationActor_BengChuangBase = 5,
	EValidationActor_RotateBengChuangBase = 6,
	EValidationActor_SpringBoard = 7,
	EValidationActor_MAX = 8
};

// Object: Enum MoeGameCore.EMoeAbnormalDataType
enum class EMoeAbnormalDataType : uint8_t
{
	EAbnormal_MaxSpeed = 1,
	EAbnormal_MaxInputSpeed = 2,
	EAbnormal_WorldGravity = 3,
	EAbnormal_GravityScale = 4,
	EAbnormal_Is_NoCollision = 5,
	EAbnormal_RPC_Frame = 6,
	EAbnormal_Add_Impulse = 7,
	EAbnormal_Add_Force = 8,
	EAbnormal_Actor_Scale = 9,
	EAbnormal_Actor_Relative_Scale = 10,
	EAbnormal_TimeDilation = 11,
	EAbnormal_Set_ActorLocation = 12,
	EAbnormal_JumpZVelocity = 13,
	EAbnormal_TimeModify = 14,
	EAbnormal_RPC_Decrypt_Error = 15,
	EAbnormal_Actor_Location = 16,
	EAbnormal_SceneActor_Is_NoCollision = 17,
	EAbnormal_MAX = 18
};

// Object: Enum MoeGameCore.EMoeRenderSignificanceLevel
enum class EMoeRenderSignificanceLevel : uint8_t
{
	LevelDefault = 0,
	Cinematic = 1,
	Epic = 2,
	High = 3,
	Medium = 4,
	Low = 5,
	EMoeRenderSignificanceLevel_MAX = 6
};

// Object: Enum MoeGameCore.EMoeSignificanceLevel
enum class EMoeSignificanceLevel : uint8_t
{
	Default = 0,
	Extreme = 1,
	Ultra = 2,
	High = 3,
	Medium = 4,
	Low = 5,
	OutsideExtreme = 6,
	OutsideUltra = 7,
	OutsideHigh = 8,
	OutsideMedium = 9,
	OutsideLow = 10,
	Max = 11
};

// Object: Enum MoeGameCore.EMoeSignificancePlaceableActorGroup
enum class EMoeSignificancePlaceableActorGroup : uint8_t
{
	GroupDefault = 0,
	GroundActor = 1,
	PropActor = 2,
	GearActor = 3,
	ItemActor = 4,
	DecorateActor = 5,
	LogicActor = 6,
	HomeInteractiveActor = 7,
	OtherPlaceableActor = 8,
	SP_POIActorBase = 9,
	SP_PropsBaseActor = 10,
	SP_ProjectileBaseActor = 11,
	SP_BuildingActorBase = 12,
	Farm_FarmActorBase = 13,
	Max = 14
};

// Object: Enum MoeGameCore.EMoeSignificanceCharacterGroup
enum class EMoeSignificanceCharacterGroup : uint8_t
{
	GroupDefault = 0,
	CommunityGameCharacter = 10,
	MainGameCharacter = 11,
	DDPCharacter = 12,
	MonsterCharacter = 13,
	NR3EGameCharacter = 14,
	FPSGameCharacter = 15,
	SPGameCharacter = 16,
	SPGameMonsterCharacter = 17,
	SPGameSummonCharacter = 18,
	OtherCharacter = 19,
	Max = 20
};

// Object: Enum MoeGameCore.EMoeSignificanceType
enum class EMoeSignificanceType : uint8_t
{
	MoeCharacter = 0,
	PlaceableActor = 1,
	MoeCharBillboard = 2,
	TYCObj = 3,
	Count = 4,
	EMoeSignificanceType_MAX = 5
};

// Object: Enum MoeGameCore.EMoeActorLODStrategy
enum class EMoeActorLODStrategy : uint8_t
{
	DistanceAndView = 0,
	DistanceViewWithLimitCount = 1,
	DistanceViewWithLimitCountV2 = 2,
	EMoeActorLODStrategy_MAX = 3
};

// Object: Enum MoeGameCore.ESignificanceConfigType
enum class ESignificanceConfigType : uint8_t
{
	Default = 0,
	QualityLevel = 1,
	SpecialScene = 2,
	ESignificanceConfigType_MAX = 3
};

// Object: Enum MoeGameCore.EMoeSigQualityLevel
enum class EMoeSigQualityLevel : uint8_t
{
	Default = 0,
	Low = 1,
	Middle = 2,
	High = 3,
	VeryHigh = 4,
	Count = 5,
	EMoeSigQualityLevel_MAX = 6
};

// Object: Enum MoeGameCore.EAISkillType
enum class EAISkillType : uint8_t
{
	Auto = 0,
	Target = 1,
	Pos = 2,
	Directional = 3,
	Track = 4,
	EAISkillType_MAX = 5
};

// Object: Enum MoeGameCore.ESkillSlot5
enum class ESkillSlot5 : uint8_t
{
	None = 0,
	InLevelInvite = 1,
	ESkillSlot5_MAX = 2
};

// Object: Enum MoeGameCore.ESkillSlot4
enum class ESkillSlot4 : uint8_t
{
	None = 0,
	Vehicle = 1,
	Temp2 = 2,
	Temp3 = 3,
	HomeInteractive = 4,
	ExclusiveGame = 5,
	EasterEggUse = 6,
	Grab = 7,
	Follow = 8,
	InMonkiBubbles = 9,
	InLevelInvite = 10,
	ESkillSlot4_MAX = 11
};

// Object: Enum MoeGameCore.ESkillSlot3
enum class ESkillSlot3 : uint8_t
{
	None = 0,
	Prop = 1,
	Grab = 2,
	Follow = 3,
	BagProp = 4,
	ESkillSlot3_MAX = 5
};

// Object: Enum MoeGameCore.ESkillSlot2
enum class ESkillSlot2 : uint8_t
{
	Sprint = 0,
	ExtraProp = 1,
	GameplayAbility = 2,
	ESkillSlot2_MAX = 3
};

// Object: Enum MoeGameCore.ESkillSlot1
enum class ESkillSlot1 : uint8_t
{
	Dive = 0,
	ESkillSlot1_MAX = 1
};

// Object: Enum MoeGameCore.ESkillSlot0
enum class ESkillSlot0 : uint8_t
{
	Jump = 0,
	Vehicle = 1,
	Shout = 2,
	Follow = 3,
	Tricycle = 4,
	Swing = 5,
	ExclusiveVehicle = 6,
	TankShoot = 7,
	FlySprint = 8,
	SeesawSpeedup = 9,
	ESkillSlot0_MAX = 10
};

// Object: Enum MoeGameCore.ESkillSlots
enum class ESkillSlots : uint16_t
{
	Slot0 = 0,
	Slot1 = 1,
	Slot2 = 2,
	Slot3 = 3,
	Slot4 = 4,
	InvalidSlot = 255,
	ESkillSlots_MAX = 256
};

// Object: Enum MoeGameCore.EMoeSoundType
enum class EMoeSoundType : uint8_t
{
	MUSIC = 0,
	SFX = 1,
	VOICE = 2,
	DEFAULT = 3,
	INVALID = 4,
	EMoeSoundType_MAX = 5
};

// Object: Enum MoeGameCore.EMoeSPCameraShakeCustomType
enum class EMoeSPCameraShakeCustomType : uint8_t
{
	InValid = 0,
	LocX = 1,
	LocY = 2,
	LocZ = 3,
	RotPitch = 4,
	RotYaw = 5,
	RotRoll = 6,
	Fov = 7,
	EMoeSPCameraShakeCustomType_MAX = 8
};

// Object: Enum MoeGameCore.EStateOverridenFlag
enum class EStateOverridenFlag : uint8_t
{
	NoOverriden = 0,
	Update = 1,
	CanEnter = 2,
	CanEnterFrom = 4,
	CanExit = 8,
	CanExitTo = 16,
	Reset = 32,
	CanReEnter = 64,
	EStateOverridenFlag_MAX = 65
};

// Object: Enum MoeGameCore.EMoeStateKeyName
enum class EMoeStateKeyName : uint8_t
{
	StateId = 1,
	StateMachineID = 2,
	Timestamp = 3,
	EnterRole = 4,
	SequenceID = 5,
	KeepTime = 6,
	StateEffectID = 7,
	CustomInt = 8,
	CustomInt1 = 9,
	CustomInt2 = 10,
	CustomInt3 = 12,
	CustomFloat = 13,
	CustomString = 14,
	bCustomBool = 15,
	CustomVector = 16,
	CustomName = 17,
	CustomFloat2 = 18,
	CustomString2 = 19,
	bCustomBool2 = 20,
	CustomVector2 = 21,
	CustomName2 = 22,
	SpeedUpTime = 23,
	StateKeyName_Max = 24,
	EMoeStateKeyName_MAX = 25
};

// Object: Enum MoeGameCore.EStateEnterRole
enum class EStateEnterRole : uint8_t
{
	Authority = 0,
	Autonomous = 1,
	AuthorityAndAutonomous = 2,
	EStateEnterRole_MAX = 3
};

// Object: Enum MoeGameCore.EUGCActorType
enum class EUGCActorType : uint8_t
{
	None = 0,
	Placeable = 1,
	Char = 2,
	NPC = 3,
	EUGCActorType_MAX = 4
};

// Object: Enum MoeGameCore.EInteractiveDragPlaneType
enum class EInteractiveDragPlaneType : uint8_t
{
	None = 0,
	XY = 1,
	XZ = 2,
	YZ = 3,
	Max = 4
};

// Object: Enum MoeGameCore.EMoeUGCLifeCycleState
enum class EMoeUGCLifeCycleState : uint8_t
{
	Scrapped = 0,
	Running = 1,
	Pause = 2,
	EMoeUGCLifeCycleState_MAX = 3
};

// Object: Enum MoeGameCore.EMoeUGCWalletCurrencyChangeType
enum class EMoeUGCWalletCurrencyChangeType : uint8_t
{
	None = 0,
	Add = 1,
	Reduce = 2,
	EMoeUGCWalletCurrencyChangeType_MAX = 3
};

// Object: Enum MoeGameCore.EDragDetectType
enum class EDragDetectType : uint8_t
{
	All = 0,
	OnlyUp = 1,
	OnlyLeft = 2,
	OnlyRight = 3,
	Horizontal = 4,
	Vertical = 5,
	EDragDetectType_MAX = 6
};

// Object: Enum MoeGameCore.EVehicleOperateStatusCode
enum class EVehicleOperateStatusCode : uint8_t
{
	PV_Success = 0,
	PV_InvalidPlayer = 1,
	PV_AlreadyHasDriver = 2,
	PV_AlreadyInVehicle = 3,
	PV_PassengersIsFull = 4,
	PV_HasNotFreeSeat = 5,
	PV_NotHasDriverSeat = 6,
	PV_PlayerNotInVehicle = 7,
	PV_MotionStateError = 8,
	PV_MAX = 9
};

// Object: Enum MoeGameCore.EMoeMotionState
enum class EMoeMotionState : uint8_t
{
	Start = 0,
	Stop = 1,
	Reverse = 2,
	EMoeMotionState_MAX = 3
};

// Object: Enum MoeGameCore.EMoeActionInputMode
enum class EMoeActionInputMode : uint8_t
{
	World = 0,
	Camera = 1,
	Camera3D = 2,
	Pawn = 3,
	EMoeActionInputMode_MAX = 4
};

// Object: Enum MoeGameCore.EMonsterParamType
enum class EMonsterParamType : uint8_t
{
	MonsterLevel = 0,
	EMonsterParamType_MAX = 1
};

// Object: Enum MoeGameCore.ENPCStateEnum
enum class ENPCStateEnum : uint8_t
{
	None = 0,
	InitialState = 1,
	EnterGame = 2,
	Idle = 3,
	Searching = 4,
	PlayerFoundReaction = 5,
	Chasing = 6,
	PrepareAttack = 7,
	Attacking = 8,
	Stun = 9,
	Escape = 10,
	Wander = 11,
	Jump = 12,
	JumpDown = 13,
	Slide = 14,
	Ragdoll = 15,
	Move = 16,
	Skill = 17,
	Death = 18,
	AfterAttack = 19,
	Patrol = 20,
	FirstSkill = 21,
	SecondSkill = 22,
	ThirdSkill = 23,
	ChestMonsterStateStart = 64,
	ChestMonsterStateIdle = 65,
	ChestMonsterStateMove = 66,
	ChestMonsterStatePatrol = 67,
	ChestMonsterStateChasing = 68,
	ChestMonsterStatePreAttack = 69,
	ChestMonsterStateAttacking = 70,
	ChestMonsterStatePostAttack = 71,
	ChestMonsterStateStun = 72,
	ChestMonsterStateFreeze = 73,
	ChestMonsterStateDead = 74,
	ChestMonsterStateAddition1 = 75,
	ChestMonsterStateAddition2 = 76,
	ChestMonsterStateAddition3 = 77,
	ChestMonsterStateEnd = 78,
	ChestPVEMonsterStateStart = 96,
	ChestPVEMonsterStateIdle = 97,
	ChestPVEMonsterStateMove = 98,
	ChestPVEMonsterStatePatrol = 99,
	ChestPVEMonsterStateChasing = 100,
	ChestPVEMonsterStatePreAttack = 101,
	ChestPVEMonsterStateAttacking = 102,
	ChestPVEMonsterStatePostAttack = 103,
	ChestPVEMonsterStateStun = 104,
	ChestPVEMonsterStateFreeze = 105,
	ChestPVEMonsterStateDead = 106,
	ChestPVEMonsterStateAddition1 = 107,
	ChestPVEMonsterStateAddition2 = 108,
	ChestPVEMonsterStateAddition3 = 109,
	ChestPVEMonsterStateEnd = 110,
	Max = 255
};

// Object: Enum MoeGameCore.EOperatorType
enum class EOperatorType : uint8_t
{
	Add = 0,
	Subtract = 1,
	Multiply = 2,
	Divide = 3,
	EOperatorType_MAX = 4
};

// Object: Enum MoeGameCore.EMoePerformAnalyseType
enum class EMoePerformAnalyseType : uint8_t
{
	MoveCount = 0,
	OverlapCount = 1,
	TickCount = 2,
	SpawnCount = 3,
	NetworkCount = 4,
	EMoePerformAnalyseType_MAX = 5
};

// Object: Enum MoeGameCore.EInfluenceMovableType
enum class EInfluenceMovableType : uint8_t
{
	None = 0,
	Static = 1,
	Movable = 2,
	EInfluenceMovableType_MAX = 3
};

// Object: Enum MoeGameCore.EMidJoinPlayerType
enum class EMidJoinPlayerType : uint8_t
{
	MID_JOIN_STATE_NONE = 0,
	MID_JOIN_STATE_JOIN = 1,
	MID_JOIN_STATE_MAX = 2
};

// Object: Enum MoeGameCore.EBattleEventType
enum class EBattleEventType : uint8_t
{
	BET_Unknown = 0,
	BET_CRASH_CNT = 1,
	BET_LEVEL_RANK = 2,
	BET_QUALIFIED_CNT = 3,
	BET_REACH_END_CNT = 4,
	BET_REACH_END_TIME = 5,
	BET_IS_TEAM_MATCH = 6,
	BET_CROWN_CNT = 7,
	BET_IS_COUNTDOWN_FINISH = 8,
	BET_CROWN_HIT_CNT = 9,
	BET_IS_PLAY_CROWN = 10,
	BET_CATCH_CNT = 11,
	BET_SURVIVE_CNT = 12,
	BET_LEVEL_RESULT = 13,
	BET_CHAMPION_LEVEL = 14,
	BET_IS_GIVEUP = 15,
	BET_TEAM_RANK = 16,
	BET_TEAM_SCORE = 17,
	BET_TEAM_GLOBAL_RANK = 18,
	BET_JUMP_CNT = 19,
	BET_DIVE_CNT = 20,
	BET_PLAY_SCORE = 21,
	BET_RANK_IN_TEAM = 22,
	BET_OTHER_TEAM_SCORE = 23,
	BET_SELF_OUT_IN_TEAM = 24,
	BET_TEAM_MEMBER_OUT = 25,
	BET_KILL_TIMES = 26,
	BET_DEAD_TIMES = 27,
	BET_SURVIVAL_TIME = 28,
	BET_HUG_PLAYER_CNT = 29,
	BET_RIDE_CAROUSEL_CNT = 30,
	BET_STACK_PLAYER_CNT = 31,
	BET_PLAY_ACTION_CNT = 32,
	BET_RIDE_BALLOON_CNT = 33,
	BET_FIND_LOBBY_EGG_CNT = 34,
	BET_DOUBLE_INTERACTION_CNT = 35,
	BET_PLAY_EXPRESSION_CNT = 36,
	BET_SEESAW_WITH_OTHERS = 37,
	BET_SIT_ON_BENCH = 38,
	BET_ON_BALANCE_BALL_PLAYER_CNT = 39,
	BET_ON_BALANCE_BALL_DURATION_TIME = 40,
	BET_PLAY_PIRATE_SHIP_CNT = 41,
	BET_BATTLE_USE_TIME = 42,
	BET_SIT_ON_BENCH_PLAYER_CNT = 43,
	BET_LEVEL_CAMP_TYPE = 44,
	BET_LEVEL_CAMP_SIDE = 45,
	BET_LEVEL_START_TIMR = 46,
	BET_LEVEL_CAMP_AI_NUM = 47,
	BET_LEVEL_END_REASON = 48,
	BET_LEVEL_INPUT_SCORE = 49,
	BET_LEVEL_PLAYER_INDEX = 50,
	BET_STUNNED_TIMES = 51,
	BET_STUNNING_TIMES = 52,
	BET_KNOCKING_TIMES = 53,
	BET_STUNNING_TIMES_PINGPONG_PADDLE = 54,
	BET_TASER_USE = 55,
	BET_SURVIVAL_TIME_SECOND = 56,
	BET_SURVIVAL_TIME_MINUTE = 57,
	BET_TEAM_MEMBER_REACH_END_CNT = 58,
	BET_SELF_OUT_IN_LEVEL = 59,
	BET_COMPLETE_TASK = 61,
	BET_MEETING_VOTE = 62,
	BET_USE_SECRETTUNNEL = 63,
	BET_INNITIATE_REPORT = 64,
	BET_COMPLETE_TASK_AS_GHOST = 65,
	BET_VOTE_EXPEL_WOLF = 66,
	BET_SPEAK = 67,
	BET_PUBLISH_URGENT_TASK = 68,
	BET_PICK_STAR = 71,
	BET_KILL_KILLER = 72,
	BET_KILL_GUARD = 73,
	BET_KILL_DOLL = 74,
	BET_CATCH_DOLL_WITH_TRAP = 75,
	BET_CATCH_GUARD_WITH_TRAP = 76,
	BET_RESCUE_DOLL = 77,
	BET_RESCUE_GUARD = 78,
	BET_USE_STEALTH_SKILL_CNT = 79,
	BET_CAMOUFLAGE_ITEM_ID = 80,
	BET_USE_EXPLORATION_ITEM = 81,
	BET_TRANSFORM = 82,
	BET_USE_SCANSKILL_SEARCH_PRETENDER = 83,
	BET_CAMP_SURVIVE_CNT = 84,
	BET_FIND_LOBBY_EGG_KIND = 85,
	BET_ON_BALANCE_BALL_WITH_OTHERS = 86,
	BET_ON_BALANCE_BALL = 87,
	BET_USE_TELEPORT = 88,
	BET_BEACH_CHAIR = 89,
	BET_SWIM_POOL_SLIDE = 90,
	BET_WATER_BALL = 91,
	BET_PICK_IRON_TERMINATOR_BOX = 92,
	BET_RIDE_TRANSPORT_VEHICLE_TIMES = 93,
	BET_KILL_TIMES_ROCKET_LAUNCHER = 94,
	BET_SURVIVAL_STAGE = 95,
	BET_SUM_RANK = 96,
	BET_INFECT_TIMES = 97,
	BET_SAVE_TEAMMATE_TIMES = 98,
	BET_TOTAL_DAMAGE_VALUE = 99,
	BET_KILL_TIMES_AS_HERO = 100,
	BET_INFECT_TIMES_HERO = 101,
	BET_MOVE_BY_STAR_SLINGSHOT = 102,
	BET_KILL_TIMES_TEAM1 = 103,
	BET_KILL_TIMES_TEAM2 = 104,
	BET_KILL_TIMES_TEAM3 = 105,
	BET_SAVE_TEAMMATE_TIMES_TEAM1 = 106,
	BET_SAVE_TEAMMATE_TIMES_TEAM2 = 107,
	BET_SAVE_TEAMMATE_TIMES_TEAM3 = 108,
	BET_TOTAL_DAMAGE_VALUE_TEAM1 = 109,
	BET_TOTAL_DAMAGE_VALUE_TEAM2 = 110,
	BET_TOTAL_DAMAGE_VALUE_TEAM3 = 111,
	BET_SURVIVAL_TIME_TEAM1 = 112,
	BET_SURVIVAL_TIME_TEAM2 = 113,
	BET_SURVIVAL_TIME_TEAM3 = 114,
	BET_SUM_RANK_TEAM1 = 115,
	BET_SUM_RANK_TEAM2 = 116,
	BET_SUM_RANK_TEAM3 = 117,
	BET_AHEAD_SECOND_PLAYER = 118,
	BET_NPC_TALK_ONE_TIME = 119,
	BET_NPC_TALK_ONE_STEP = 120,
	BET_PLAY_BLOCK_GRAB = 121,
	BET_GET_XIAO_XIN_CAR = 122,
	BET_GET_BUBBLE_GUN = 123,
	BET_PLAY_GUN_BUBBLE = 124,
	BET_SEESAW_CREATE_HEART = 125,
	BET_USE_ITEM_CNT = 126,
	BET_USE_RUN_SKILL_CNT = 127,
	BET_COMPETITION_RANK = 128,
	BET_PLAY_BEACH_ChAIR_LONG_TIME = 129,
	BET_WATER_BALL_GOAL = 130,
	BET_RIDE_CAROUSEL_WITH_OTHERS = 131,
	BET_BASKETBALL_HIT_CNT = 132,
	BET_ARCHERY_HIT_CNT = 133,
	BET_USE_DRIFTING_TIMES = 134,
	BET_HIT_GROUND_ACCELERATOR_BELT_TIMES = 135,
	BET_PASSTHROUGH_ACCELERATOR_LAPS_TIMES = 136,
	BET_USE_PARAGLIDING_TIMES = 137,
	BET_USE_BICYLE_TIMES = 138,
	BET_USE_SKIING_TIMES = 139,
	BET_TYC_KILL_MONSTER_NUM = 159,
	BET_TYC_GET_COIN_FROM_BANK = 160,
	BET_TYC_CONSUME_COIN_ON_BUILDING = 161,
	BET_TYC_PLAYER_REBIRTH_NUM = 162,
	BET_TYC_PLAYER_BUILD_NUM = 163,
	BET_TYC_PLAYER_GET_COIN_NUM = 164,
	BET_TYC_KILL_BOSS_NUM = 165,
	BET_TYC_GET_COIN_FROM_PvP = 166,
	BET_TYC_CONSUME_DROP_ITEM_NUM = 167,
	BET_TD_USE_SKILL_ON_MONITOR = 168,
	BET_PLAY_NEW_YEAR_CALL = 169,
	BET_NPC_TALK_DIALOGUE_ID = 170,
	BET_TOTAL_LEVEL_COUNT = 171,
	BET_PASS_LEVEL_COUNT = 172,
	BET_LEVEL_DIFFICULTY_INDEX = 173,
	BET_TALENT_COUNT1 = 174,
	BET_TALENT_COUNT2 = 175,
	BET_TALENT_COUNT3 = 176,
	BET_ROGUELIKE_TASK_TALENT_REWARD = 177,
	BET_ROGUELIKE_SETTLEMENT_SCORE = 178,
	BET_TD_PLAYER_CONTRIBUTE = 180,
	BET_TD_USE_SKILL_CONSUME_ON_MONITOR = 181,
	BET_TD_USE_TOTAL_SCORE = 182,
	BET_USE_GAMEPLAY_PROP_ID = 183,
	BET_PLAY_FESTIVE_LANTERN = 184,
	BET_SPECIFIC_MAP_ENTRANCE = 185,
	BET_LEVEL_EXCEPT_PLAYER_CNT = 186,
	BET_LEVEL_WARM_SCORE = 187,
	BET_LEVEL_CHASE_PROGRESS = 188,
	BET_LEVEL_CHASE_DAMAGE = 189,
	BET_LEVEL_SEEKER_KILL = 190,
	BET_LEVEL_KILLER_KILL_CNT = 191,
	BET_USE_KARTRACING_TIMES = 192,
	BET_INLEVEL_COST_COIN = 193,
	BET_INLEVEL_GET_COIN = 194,
	BET_SEND_DANMU = 195,
	BET_RIDING_DRAGON = 196,
	BET_ULTRAMAN_DAMAGE_VALUE = 197,
	BET_ULTRAMAN_USE_ITEM = 198,
	BET_ULTRAMAN_CHAT_CNT = 199,
	BET_MAX = 200
};

// Object: Enum MoeGameCore.ERoguelikeStrongholdState
enum class ERoguelikeStrongholdState : uint8_t
{
	E_UnOccupy = 0,
	E_Fight = 1,
	E_UnFight = 2,
	E_MAX = 3
};

// Object: Enum MoeGameCore.ERoguelikeProcessType
enum class ERoguelikeProcessType : uint8_t
{
	E_LevelStart = 0,
	E_EnableSpawn = 1,
	E_SpawnFinish = 2,
	E_AllMonsterDie = 3,
	E_EnableTriggerArea = 4,
	E_TeleportPlayer = 5,
	E_AccumulateKillEnemy = 6,
	E_EnableStronghold = 7,
	E_LevelWin = 8,
	E_LevelLose = 9,
	E_MAX = 10
};

// Object: Enum MoeGameCore.ERoguelikeSpecialEventType
enum class ERoguelikeSpecialEventType : uint8_t
{
	E_None = 0,
	E_ChestEvent = 1,
	E_MAX = 2
};

// Object: Enum MoeGameCore.EPlacePropsSpawnType
enum class EPlacePropsSpawnType : uint8_t
{
	E_Fixed = 0,
	E_Random = 1,
	E_MAX = 2
};

// Object: Enum MoeGameCore.EPlacePropsType
enum class EPlacePropsType : uint8_t
{
	E_BloodBag = 0,
	E_Battery = 1,
	E_MAX = 2
};

// Object: Enum MoeGameCore.ERoguelikeMapStyle
enum class ERoguelikeMapStyle : uint8_t
{
	E_Island = 0,
	E_Ice = 1,
	E_Fire = 2,
	E_Other = 3,
	E_MAX = 4
};

// Object: Enum MoeGameCore.ERoguelikeNotifyType
enum class ERoguelikeNotifyType : uint8_t
{
	E_OccupyByTime = 0,
	E_AnnihilateByTime = 1,
	E_ServivalByTime = 2,
	E_EliteByTime = 3,
	E_EscortByPercent = 4,
	E_MiningByTime = 5,
	E_MAX = 6
};

// Object: Enum MoeGameCore.ERoguelikePropsType
enum class ERoguelikePropsType : uint8_t
{
	E_None = 0,
	E_Gold = 1,
	E_Ex = 2,
	E_Props = 3,
	E_Chest = 4,
	E_MAX = 5
};

// Object: Enum MoeGameCore.ERoguelikeChestGetType
enum class ERoguelikeChestGetType : uint8_t
{
	E_LevelUp = 0,
	E_ChestMonster = 1,
	E_Challenge = 2,
	E_LevelEnd = 3,
	E_Specialize = 4,
	E_Mining = 5,
	E_Chest = 6,
	E_MAX = 7
};

// Object: Enum MoeGameCore.ERoguelikePlayerState
enum class ERoguelikePlayerState : uint8_t
{
	E_Playing = 0,
	E_CatchChest = 1,
	E_ChooseLevel = 2,
	E_Shopping = 3,
	E_MAX = 4
};

// Object: Enum MoeGameCore.ERoguelikeLevelMonsterAffixType
enum class ERoguelikeLevelMonsterAffixType : uint8_t
{
	E_None = 0,
	E_DoubleHp = 1,
	E_OneHitKill = 2,
	E_AttackUp = 3,
	E_FastMonster = 4,
	E_MonsterBomb = 5,
	E_MAX = 6
};

// Object: Enum MoeGameCore.ERoguelikeLevelPlayerAffixType
enum class ERoguelikeLevelPlayerAffixType : uint8_t
{
	E_None = 0,
	E_UnlimitedFire = 1,
	E_FastMan = 2,
	E_IronMan = 3,
	E_BountyHunter = 4,
	E_SpeadUpBySkill = 5,
	E_ExUp = 6,
	E_SkillRechargeUp = 7,
	E_StandUp = 8,
	E_MAX = 9
};

// Object: Enum MoeGameCore.ERoguelikeLevelTaskType
enum class ERoguelikeLevelTaskType : uint8_t
{
	E_Unkown = 0,
	E_Elite = 1,
	E_Survival = 2,
	E_Guard = 3,
	E_Escort = 4,
	E_Occupy = 5,
	E_Shop = 6,
	E_Boss = 7,
	E_Annihilate = 8,
	E_Prepare = 9,
	E_Mining = 10,
	E_Chest = 11,
	E_MAX = 12
};

// Object: Enum MoeGameCore.ERoguelikeLevelType
enum class ERoguelikeLevelType : uint8_t
{
	E_Level0 = 0,
	E_Level1 = 1,
	E_Level2 = 2,
	E_Level3 = 3,
	E_Level4 = 4,
	E_Level5 = 5,
	E_Level6 = 6,
	E_Level7 = 7,
	E_Level8 = 8,
	E_Level9 = 9,
	E_Level10 = 10,
	E_Level11 = 11,
	E_Level12 = 12,
	E_Level13 = 13,
	E_Level14 = 14,
	E_Level15 = 15,
	E_Level16 = 16,
	E_Level17 = 17,
	E_Level18 = 18,
	E_Level19 = 19,
	E_Level20 = 20,
	E_Level21 = 21,
	E_Level22 = 22,
	E_Level23 = 23,
	E_Level24 = 24,
	E_Level25 = 25,
	E_Level26 = 26,
	E_Level27 = 27,
	E_Level28 = 28,
	E_Level29 = 29,
	E_Level30 = 30,
	E_Level31 = 31,
	E_Level32 = 32,
	E_Level33 = 33,
	E_Level34 = 34,
	E_Level35 = 35,
	E_Level36 = 36,
	E_Level37 = 37,
	E_Level38 = 38,
	E_Level39 = 39,
	E_Level40 = 40,
	E_Level41 = 41,
	E_Level42 = 42,
	E_Level43 = 43,
	E_Level44 = 44,
	E_Level45 = 45,
	E_Level46 = 46,
	E_Level47 = 47,
	E_Level48 = 48,
	E_Level49 = 49,
	E_Level50 = 50,
	E_Level51 = 51,
	E_Level52 = 52,
	E_Level53 = 53,
	E_Level54 = 54,
	E_Level55 = 55,
	E_Level56 = 56,
	E_Level57 = 57,
	E_Level58 = 58,
	E_Level59 = 59,
	E_Level60 = 60,
	E_Level61 = 61,
	E_Level62 = 62,
	E_Level63 = 63,
	E_Level64 = 64,
	E_Level65 = 65,
	E_Level66 = 66,
	E_Level67 = 67,
	E_Level68 = 68,
	E_Level69 = 69,
	E_Level70 = 70,
	E_Level71 = 71,
	E_Level72 = 72,
	E_Level73 = 73,
	E_Level74 = 74,
	E_Level75 = 75,
	E_Level76 = 76,
	E_Level77 = 77,
	E_Level78 = 78,
	E_Level79 = 79,
	E_Level80 = 80,
	E_Level81 = 81,
	E_Level82 = 82,
	E_Level83 = 83,
	E_Level84 = 84,
	E_Level85 = 85,
	E_Level86 = 86,
	E_Level87 = 87,
	E_Level88 = 88,
	E_Level89 = 89,
	E_Level90 = 90,
	E_Level91 = 91,
	E_Level92 = 92,
	E_Level93 = 93,
	E_Level94 = 94,
	E_Level95 = 95,
	E_Level96 = 96,
	E_Level97 = 97,
	E_Level98 = 98,
	E_Level99 = 99,
	E_Level100 = 100,
	E_Level101 = 101,
	E_Level102 = 102,
	E_Level103 = 103,
	E_Level104 = 104,
	E_Level105 = 105,
	E_Level106 = 106,
	E_Level107 = 107,
	E_Level108 = 108,
	E_Level109 = 109,
	E_Level110 = 110,
	E_Level111 = 111,
	E_Level112 = 112,
	E_Level113 = 113,
	E_Level114 = 114,
	E_Level115 = 115,
	E_Level116 = 116,
	E_Level117 = 117,
	E_Level118 = 118,
	E_Level119 = 119,
	E_Level120 = 120,
	E_Level121 = 121,
	E_Level122 = 122,
	E_Level123 = 123,
	E_Level124 = 124,
	E_Level125 = 125,
	E_Level126 = 126,
	E_Level127 = 127,
	E_Level128 = 128,
	E_Level129 = 129,
	E_Level130 = 130,
	E_Level131 = 131,
	E_Level132 = 132,
	E_Level133 = 133,
	E_Level134 = 134,
	E_Level135 = 135,
	E_Level136 = 136,
	E_Level137 = 137,
	E_Level138 = 138,
	E_Level139 = 139,
	E_Level140 = 140,
	E_Level141 = 141,
	E_Level142 = 142,
	E_Level143 = 143,
	E_Level144 = 144,
	E_Level145 = 145,
	E_Level146 = 146,
	E_Level147 = 147,
	E_Level148 = 148,
	E_Level149 = 149,
	E_Level150 = 150,
	E_Level151 = 151,
	E_Level152 = 152,
	E_Level153 = 153,
	E_Level154 = 154,
	E_Level155 = 155,
	E_Level156 = 156,
	E_Level157 = 157,
	E_Level158 = 158,
	E_Level159 = 159,
	E_Level160 = 160,
	E_Level161 = 161,
	E_Level162 = 162,
	E_Level163 = 163,
	E_Level164 = 164,
	E_Level165 = 165,
	E_Level166 = 166,
	E_Level167 = 167,
	E_Level168 = 168,
	E_Level169 = 169,
	E_Level170 = 170,
	E_Level171 = 171,
	E_Level172 = 172,
	E_Level173 = 173,
	E_Level174 = 174,
	E_Level175 = 175,
	E_Level176 = 176,
	E_Level177 = 177,
	E_Level178 = 178,
	E_Level179 = 179,
	E_Level180 = 180,
	E_Level181 = 181,
	E_Level182 = 182,
	E_Level183 = 183,
	E_Level184 = 184,
	E_Level185 = 185,
	E_Level186 = 186,
	E_Level187 = 187,
	E_Level188 = 188,
	E_Level189 = 189,
	E_Level190 = 190,
	E_Level191 = 191,
	E_Level192 = 192,
	E_Level193 = 193,
	E_Level194 = 194,
	E_Level195 = 195,
	E_Level196 = 196,
	E_Level197 = 197,
	E_Level198 = 198,
	E_Level199 = 199,
	E_Level200 = 200,
	E_MAX = 201
};

// Object: Enum MoeGameCore.ERoguelikeMonsterAreaSpecialEventType
enum class ERoguelikeMonsterAreaSpecialEventType : uint8_t
{
	E_ByChallenge = 0,
	E_MAX = 1
};

// Object: Enum MoeGameCore.ERoguelikeMonsterAreaActiveType
enum class ERoguelikeMonsterAreaActiveType : uint8_t
{
	E_ByProcess = 0,
	E_BySpecialEvent = 1,
	E_MAX = 2
};

// Object: Enum MoeGameCore.ERoguelikeSpawnType
enum class ERoguelikeSpawnType : uint8_t
{
	E_ByTime = 0,
	E_ByMonsterDie = 1,
	E_MAX = 2
};

// Object: Enum MoeGameCore.ERoguelikeMapType
enum class ERoguelikeMapType : uint8_t
{
	E = 0,
	E = 1,
	E = 2,
	E = 3,
	E_50705 = 4,
	E = 5,
	E = 6,
	E = 7,
	E = 8,
	E = 9,
	E = 10,
	E = 11,
	E = 12,
	E = 13,
	E = 14,
	E = 15,
	E_50705 = 16,
	E_50705 = 17,
	E_50705 = 18,
	E_50705 = 19,
	E = 20,
	E = 21,
	E = 22,
	E = 23,
	E = 24,
	E = 25,
	E = 26,
	E = 27,
	E = 28,
	E_MAX = 29
};

// Object: Enum MoeGameCore.ERoguelikeMonsterType
enum class ERoguelikeMonsterType : uint8_t
{
	E_SoulNinjia = 0,
	E_RedMan = 1,
	E_Hulk = 2,
	E_Crab = 3,
	E_CrazyBaby = 4,
	E_Succubus = 5,
	E_TreasureCrab = 6,
	E_Bomb = 7,
	E_FlyFrog = 8,
	E_IronTerminator = 9,
	E_BlueTerminator = 10,
	E_BOSS = 11,
	E_Crystal = 12,
	E_BusinessMan = 13,
	E_ExplosiveStone = 14,
	E_MagmaStone = 15,
	E_ElectricStone = 16,
	E_IceStone = 17,
	E_BlackholeStone = 18,
	E_MAX = 19
};

// Object: Enum MoeGameCore.ERoguelikeItemType
enum class ERoguelikeItemType : uint8_t
{
	Unknown = 0,
	Artifact = 1,
	MainCollectable = 2,
	SubCollectable = 3,
	SpecialCollectable = 4,
	Normal = 5,
	Currency = 6,
	MarkBoard = 7,
	CharacterAttr = 8,
	ERoguelikeItemType_MAX = 9
};

// Object: Enum MoeGameCore.ERoguelikeGameDetailState
enum class ERoguelikeGameDetailState : uint8_t
{
	Default = 0,
	Loading = 1,
	Gaming = 2,
	Reward = 3,
	ERoguelikeGameDetailState_MAX = 4
};

// Object: Enum MoeGameCore.ERoguelikeGameState
enum class ERoguelikeGameState : uint8_t
{
	Default = 0,
	Prepare = 1,
	Gaming = 2,
	Reward = 3,
	ERoguelikeGameState_MAX = 4
};

// Object: Enum MoeGameCore.EShootAttrType
enum class EShootAttrType : uint16_t
{
	Unknown = 0,
	Hp = 1,
	MaxHp = 2,
	Invincible = 3,
	CriticalRate = 4,
	CriticalDamage = 5,
	ShieldValue = 6,
	MaxShieldValue = 7,
	RoleLevel = 8,
	AttackDamage = 9,
	AttackRate = 10,
	AttackSpeed = 11,
	InfiniteBullet = 12,
	NextAttackCritical = 13,
	DeadTime = 14,
	BeHelpStartTime = 15,
	MyBurningDamageRate = 16,
	AddGetDamageRate = 17,
	ClipInMagazine = 18,
	ClipInMagazineMulti = 19,
	ReloadSpeed = 20,
	ReloadSpeedEmpty = 21,
	RemainingFallTime = 22,
	AlreadySaveCount = 23,
	InterruptHelpTime = 24,
	DyingState = 25,
	DyingStateEnterTime = 26,
	LeftLives = 27,
	NextLifeRespawnTime = 28,
	BrBodyDamageReduction = 29,
	BrBodyShield = 30,
	BrHeadDamageReduction = 31,
	BrHeadShield = 32,
	BrBeGrabDamageReduction = 33,
	BrBodyMaxShield = 34,
	BrHeadMaxShield = 35,
	MoveSpeed = 36,
	BeHealedRate = 37,
	HealRate = 38,
	CharMoveSpeed = 39,
	CharMoveSpeedRate = 40,
	AddGetBombDamageRate = 41,
	CalculateCriticalInClient = 42,
	FinalAddDamage = 43,
	ReduceDamageRate = 44,
	LockedAttackRate = 45,
	LockedCriticalDamageRate = 46,
	ExShotCost = 47,
	ProhibitChangeClip = 48,
	MarkAttackRate = 49,
	ExtraColdIntervalRate = 50,
	ColdRangeRate = 51,
	FreezeAddDamage = 52,
	ExtraElectricIntervalRate = 53,
	ElectricRangeRate = 54,
	ElectricDamageRate = 55,
	ElectricExtraTarget = 56,
	ElectricStoneBuffCount = 57,
	HealByDamageLimit = 59,
	HealByDamageProb = 60,
	HealByDamageRate = 61,
	HealByKillValue = 62,
	BloodShieldValue = 63,
	MaxBloodShieldValue = 64,
	TempShieldValue = 65,
	LandmineExInterval = 66,
	LandmineExDamage = 67,
	LandmineExBombDiameter = 68,
	LandmineAddBuffId = 69,
	AddBombDamageRate = 70,
	AddBombDiameter = 71,
	ExBombDamageRate = 72,
	ExBombDiameterRate = 73,
	SubProjectileDamageRate = 74,
	RechargeCount = 75,
	RechargeMaxCount = 76,
	AddRechargeSpeed = 77,
	AddStandRechargeSpeed = 78,
	IsRecharge = 79,
	LevelStartGetGoldRate = 80,
	AddGetGoldRate = 81,
	ShopDiscountRate = 82,
	ClientNowGold = 83,
	StandTime = 84,
	CounterAttackRate = 85,
	CounterAttackDoubleProb = 86,
	AddCounterAttackDamageRate = 87,
	PoisonDamageRate = 88,
	PoisonExtraStack = 89,
	FarAddDamageRate = 90,
	FarAddDamageDis = 91,
	NearAddDamageRate = 92,
	NearAddDamageNearDis = 93,
	NearAddDamageFarDis = 94,
	BaseShieldMaxValue = 95,
	ExShieldMaxRate = 96,
	ShieldMaxDamage = 97,
	ShieldReduceDamage = 98,
	BaseMaxHp = 99,
	ExMaxHpRate = 100,
	MoveSpeedAddDamage = 101,
	NotCriticalStack = 102,
	SafeTime = 103,
	RevivalTime = 104,
	RevivalHpRate = 105,
	SkillRechargeShoot = 106,
	SkillRechargeAddDamageRate = 107,
	SkillRechargeKillBuff = 108,
	UnlimitedFire = 109,
	ExSkillRechargeSpeed = 110,
	SkillCDReduceRate = 111,
	AddGetExRate = 112,
	StrongColdFlag = 113,
	BuyRespawnEndTime = 114,
	TYCMoveSpeed = 115,
	TYCInjuryRatio = 116,
	TYCDamageRatio = 117,
	IsAddAmmoByTime = 118,
	ColdDamageRate = 119,
	ImproveBurnEffect = 120,
	ImproveColdEffect = 121,
	ImproveElecEffect = 122,
	ImprovePoisonEffect = 123,
	ElementAccumValue = 124,
	ElementDamageRate = 125,
	ExtraElementDamageTimes = 126,
	DamageTimeInEleExplore = 127,
	RecycleAmmoRate = 128,
	FocusCount = 129,
	PoisonMinRate = 130,
	PoisonMaxRate = 131,
	PoisonAddRateSpeed = 132,
	PoisonStayTime = 133,
	PoisonExLiveTime = 134,
	TYCDizziness = 135,
	TYCRush = 136,
	TYCBuffActionIndex = 137,
	TYCBurning = 138,
	TYCFrozen = 139,
	TYCMire = 140,
	TYCFeared = 141,
	TYCDeadAudio = 142,
	TYCCharmed = 143,
	TYCTrapInvalid = 144,
	TYCElectricShock = 145,
	TYCConcentrated = 146,
	TYCTrapDamageRatio = 147,
	TYCTrapBuffSpeedRatio = 148,
	AddWeaponFireRange = 149,
	ReducePunchX = 150,
	ReducePunchY = 151,
	ReduceHipSpread = 152,
	ReduceAimSpread = 153,
	AddBombRadiusPercent = 154,
	LevelStartGetGoldLimit = 155,
	AccumulateBurnDamage = 156,
	BurnLevelRate = 157,
	ElecLevelRate = 158,
	AutoAddAmmoRate = 159,
	UGCSkillDizziness = 160,
	UGCSkillCharmed = 161,
	UGCSkillKnockup = 162,
	UGCSkillEndure = 163,
	SPVirtualAmmo = 164,
	SPCanReloadWhileTumbling = 165,
	SPGuaranteedWeakPointHit = 166,
	SPWeaponOverloadCurrentEnergy = 167,
	SPWeaponOverloadEnergyLimit = 168,
	SPWeaponChargeDamageMultiplier = 169,
	SPWeaponOverloadEnergyGrowthMultiplier = 170,
	SPWeaponOverloadEnergyDecayMultiplier = 171,
	SPWeaponOverloaded = 172,
	SPWeaponDoNotResetCombo = 173,
	SPWeaponAutoCombo = 174,
	SPDefenseLayerCount = 175,
	SPWeaponHitWoundValue = 176,
	SPShotgunWeaponAngleSpreadMultiplier = 177,
	SPWeaponHitWoundAsWeakPoint = 178,
	Max = 179,
	VirtualAddAmmoByTime = 10150,
	VirtualAddBuffEmptyClip = 10151,
	VirtualHighShotCostSpecial = 10152,
	VirtualAddFocusCount = 10153,
	VirtualAddBuffEmptyGiveWeapon = 10154,
	VirtualAddSecondWeaponAmmo = 10155,
	VirtualStart = 10180,
	VirtualClip = 10181,
	VirtualClipRate = 10182,
	VirtualOverload = 10183,
	VirtualFireDamage = 10184,
	VirtualPoisonCircleDamage = 10186,
	VirtualAddColdInterval = 10190,
	VirtualFreezeChar = 10191,
	VirtualReduceFreezeDuration = 10192,
	VirtualCold = 10193,
	VirtualElectricDamage = 10200,
	VirtualAddElectric = 10201,
	VirtualAddBeHealedRate = 10210,
	VirtualHealedByDamage = 10211,
	VirtualHealByKill = 10212,
	VirtualAddHealByDamageRate = 10213,
	VirtualAddHealByDamageProb = 10214,
	VirtualPlaceLandmine = 10215,
	VirtualAddRechargeByTime = 10216,
	VirtualSetIsRecharge = 10217,
	VirtualAddAttackSpeed = 10218,
	VirtualCalculateStandTime = 10219,
	VirtualAddStandAttr = 10220,
	VirtualAOECounterAttack = 10221,
	VirtualAddTempShield = 10222,
	VirtualPoisonBaseWeaponDamage = 10228,
	VirtualSetPoisonParameter = 10229,
	VirtualPoisonDamage = 10230,
	VirtualPoisonSplit = 10231,
	VirtualExtraPoison = 10232,
	VirtualAddFire = 10233,
	VirtualSkillRecharge = 10234,
	VirtualCalculateSafeTime = 10235,
	VirtualSetRevival = 10236,
	VirtualSkillShootRecharge = 10237,
	VirtualUnlimitedFire = 10238,
	VirtualStandAddShield = 10239,
	VirtualAddBuffWhenDie = 10240,
	VirtualMonsterBomb = 10241,
	VirtualAddHealthByRate = 10242,
	VirtualBeginElementAccum = 10243,
	VirtualSetHealedByDamageLimit = 10244,
	VirtualBurnExplored = 10245,
	VirtualBurnLevelRate = 10246,
	VirtualElecLevelRate = 10247,
	VirtualDoColdDamage = 10248,
	VirtualBuffDuration = 10249
};

// Object: Enum MoeGameCore.EConterAttackType
enum class EConterAttackType : uint8_t
{
	ByBeHitted = 0,
	ByShieldBroken = 1,
	EConterAttackType_MAX = 2
};

// Object: Enum MoeGameCore.ECharacterClamp
enum class ECharacterClamp : uint8_t
{
	EnemyClamp = 0,
	HumanClamp = 1,
	ECharacterClamp_MAX = 2
};

// Object: Enum MoeGameCore.EShootValueType
enum class EShootValueType : uint8_t
{
	Float = 0,
	Boolean = 1,
	Integer = 2,
	EShootValueType_MAX = 3
};

// Object: Enum MoeGameCore.EIsWipeVelocity
enum class EIsWipeVelocity : uint8_t
{
	None = 0,
	WipeZ = 1,
	WipeAll = 2,
	EIsWipeVelocity_MAX = 3
};

// Object: Enum MoeGameCore.ESubLevelStatus
enum class ESubLevelStatus : uint8_t
{
	NoLevel = 0,
	UnloadingLevel = 1,
	LoadingLevel = 2,
	LevelLoaded = 3,
	LevelShowed = 4,
	LevelHided = 5,
	LoadingToHide = 6,
	ESubLevelStatus_MAX = 7
};

// Object: Enum MoeGameCore.ETYCActorType
enum class ETYCActorType : uint8_t
{
	TYCActorType_Player = 0,
	TYCActorType_Monster = 1,
	TYCActorType_Drone = 2,
	TYCActorType_Tower = 3,
	TYCActorType_Building = 4,
	TYCActorType_Trigger = 5,
	TYCActorType_NewbieWall = 6,
	TYCActorType_Trap = 7,
	TYCActorType_GhostGuideMonster = 8,
	TYCActorType_AirWall = 9,
	TYCActorType_MAX = 10
};

// Object: Enum MoeGameCore.ETYCSignificanceType
enum class ETYCSignificanceType : uint8_t
{
	Tower = 0,
	Monster = 1,
	Trigger = 2,
	Bullet = 3,
	ETYCSignificanceType_MAX = 4
};

// Object: Enum MoeGameCore.EInteractionType
enum class EInteractionType : uint8_t
{
	Dialogue = 1,
	EInteractionType_MAX = 2
};

// Object: Enum MoeGameCore.EUGCAIInputType
enum class EUGCAIInputType : uint8_t
{
	Jump = 1,
	StopJump = 2,
	SprintedMove = 3,
	Dive = 4,
	SprintState = 5,
	WalkState = 6,
	Grab = 7,
	Slide = 8,
	EUGCAIInputType_MAX = 9
};

// Object: Enum MoeGameCore.EMoeUGCVVehicleMovingSfxType
enum class EMoeUGCVVehicleMovingSfxType : uint8_t
{
	None = 0,
	ToyCar = 1,
	NormalCar = 2,
	SportsCar = 3,
	SUV = 4,
	Bus = 5,
	Truck = 6,
	EngineeringVehicle = 7,
	ElectricCar = 8,
	Bicycle = 9,
	EMoeUGCVVehicleMovingSfxType_MAX = 10
};

// Object: Enum MoeGameCore.EMoeUGCVehicleHornSfxType
enum class EMoeUGCVehicleHornSfxType : uint8_t
{
	ToyCar = 0,
	NormalCar = 1,
	SportsCar = 2,
	SUV = 3,
	Bus = 4,
	Truck = 5,
	EngineeringCar = 6,
	BicycleRing = 7,
	EMoeUGCVehicleHornSfxType_MAX = 8
};

// Object: Enum MoeGameCore.EMoeUGCVehicleSeatCameraType
enum class EMoeUGCVehicleSeatCameraType : uint8_t
{
	VehicleCamera = 0,
	SeatCamera = 1,
	EMoeUGCVehicleSeatCameraType_MAX = 2
};

// Object: Enum MoeGameCore.EMoeUGCVehicleDriveAnimType
enum class EMoeUGCVehicleDriveAnimType : uint8_t
{
	DriveAnim_Invalid = 0,
	DriveAnim_SteeringWheel = 1,
	DriveAnim_StaticHold = 2,
	DriveAnim_None = 3,
	DriveAnim_MAX = 4
};

// Object: Enum MoeGameCore.EMoeSplineGetType
enum class EMoeSplineGetType : uint8_t
{
	Location = 0,
	Rotation = 1,
	Direction = 2,
	Velocity = 3,
	EMoeSplineGetType_MAX = 4
};

// Object: Enum MoeGameCore.EMoeSplineKeyType
enum class EMoeSplineKeyType : uint8_t
{
	InputKey = 0,
	Time = 1,
	Distance = 2,
	FindClosest = 3,
	EMoeSplineKeyType_MAX = 4
};

// Object: Enum MoeGameCore.EMoeSplineCoordinateSpace
enum class EMoeSplineCoordinateSpace : uint8_t
{
	World = 0,
	Local = 1,
	EMoeSplineCoordinateSpace_MAX = 2
};

// Object: Enum MoeGameCore.EMoeDirection
enum class EMoeDirection : uint8_t
{
	XAxis = 0,
	XAxisNagetive = 1,
	YAxis = 2,
	YAxisNagetive = 3,
	ZAxis = 4,
	ZAxisNagetive = 5,
	EMoeDirection_MAX = 6
};

// Object: Enum MoeGameCore.EWeaponAimType
enum class EWeaponAimType : uint8_t
{
	SimpleSight = 0,
	Sight = 1,
	EWeaponAimType_MAX = 2
};

// Object: Enum MoeGameCore.EMoeCharCameraShakeType
enum class EMoeCharCameraShakeType : uint8_t
{
	Normal = 0,
	Walk = 1,
	Run = 2,
	Fire = 3,
	Jump = 4,
	ChangeClip = 5,
	ChangeClip_Empty = 6,
	Fire3P = 7,
	EMoeCharCameraShakeType_MAX = 8
};

// Object: Enum MoeGameCore.EWeaponStateMachineType
enum class EWeaponStateMachineType : uint8_t
{
	EWS_None = 0,
	EWS_WeaponInactive = 1,
	EWS_WeaponActive = 2,
	EWS_WeaponMoveHipFireGetReady = 3,
	EWS_WeaponFiring = 4,
	EWS_WeaponFireEnd = 5,
	EWS_WeaponClipChanging = 6,
	EWS_WeaponEquipping = 7,
	EWS_WeaponUnequipping = 8,
	EWS_WeaponAiming = 9,
	EWS_WeaponEnteringAim = 10,
	EWS_WeaponExitingAim = 11,
	EWS_WeaponAimFiring = 12,
	EWS_WeaponAimFireEnd = 13,
	EWS_WeaponFiringSkill = 14,
	EWS_WeaponAccumulatePower = 15,
	EWS_WeaponChargeStart = 16,
	EWS_WeaponChargeFiring = 17,
	EWS_WeaponChargeEnd = 18,
	EWS_WeaponOverloadedFiring = 19,
	EWS_WeaponIronSights = 20,
	EWS_WeaponIronSightsFiring = 21,
	EWS_Max = 22
};

// Object: Enum MoeGameCore.EWeaponSnapState
enum class EWeaponSnapState : uint8_t
{
	Wait = 0,
	SnapTo = 1,
	EndFire = 2,
	EWeaponSnapState_MAX = 3
};

// Object: Enum MoeGameCore.EPickPropState
enum class EPickPropState : uint8_t
{
	Default = 0,
	PickProp = 1,
	EPickPropState_MAX = 2
};

// Object: Enum MoeGameCore.EFireMode
enum class EFireMode : uint8_t
{
	Machine = 0,
	Grenade = 1,
	EFireMode_MAX = 2
};

// Object: Enum MoeGameCore.EWeaponAttachmentType
enum class EWeaponAttachmentType : uint8_t
{
	Sight = 0,
	Muzzle = 1,
	Grip = 2,
	Stock = 3,
	Magazine = 4,
	MaxWeaponAttachmentType = 5,
	EWeaponAttachmentType_MAX = 6
};

// Object: Enum MoeGameCore.EAimAssistEffectiveType
enum class EAimAssistEffectiveType : uint8_t
{
	EAimAssist_Never = 0,
	EAimAssist_Always = 1,
	EAimAssist_AnyInput = 2,
	EAimAssist_HasMoveOrRotInput = 3,
	EAimAssist_MAX = 4
};

// Object: Enum MoeGameCore.ESGWeaponAmmoType
enum class ESGWeaponAmmoType : uint8_t
{
	Default = 0,
	Light = 1,
	Heavy = 2,
	Shotgun = 3,
	Sniper = 4,
	RPG = 5,
	ESGWeaponAmmoType_MAX = 6
};

// Object: Enum MoeGameCore.ESGWeaponSkinType
enum class ESGWeaponSkinType : uint8_t
{
	Unknown = 0,
	ChangeMaterial = 1,
	ChangeTexture = 2,
	ChangeModel = 3,
	ESGWeaponSkinType_MAX = 4
};

// Object: Enum MoeGameCore.ESGWeaponHiddenState
enum class ESGWeaponHiddenState : uint16_t
{
	Unknown = 0,
	Dying = 1,
	PropHold = 2,
	Sprint = 4,
	Skydiving = 8,
	LobbyAction = 16,
	OperatingBomb = 32,
	Drive = 64,
	InHiddenGrass = 128,
	PropUse = 256,
	PoleVault = 512,
	ReqByAttr = 1024,
	ESGWeaponHiddenState_MAX = 1025
};

// Object: Enum MoeGameCore.ESGWeaponServerCheck
enum class ESGWeaponServerCheck : uint8_t
{
	Unknown = 0,
	OwnerDist = 1,
	EnemyDist = 2,
	DamageValid = 3,
	DamageInterval = 4,
	ShootCost = 5,
	ClipAmmoCount = 6,
	ChangeClipTime = 7,
	Trace = 8,
	EWP_DrivingTurret = 9,
	ESGWeaponServerCheck_MAX = 10
};

// Object: Enum MoeGameCore.ESGWeaponScopeType
enum class ESGWeaponScopeType : uint8_t
{
	EWC_Scope_Default = 0,
	EWC_Sight_Scope_1x = 1,
	EWC_Sight_Scope_2x = 2,
	EWC_Sight_Scope_4x = 3,
	EWC_Sight_Scope_6x = 4,
	EWC_Sight_Scope_8x = 5,
	EWC_MAX = 6
};

// Object: Enum MoeGameCore.ESGWeaponCategory
enum class ESGWeaponCategory : uint8_t
{
	Unknown = 0,
	EWC_Rifles = 1,
	EWC_ShotGuns = 2,
	EWC_MachineGuns = 3,
	EWC_SubMachineGuns = 4,
	EWC_RPG = 5,
	EWC_HandGuns = 6,
	EWC_SniperRifles = 7,
	EWC_ElectricGuns = 8,
	EWC_ThrowWeapons = 9,
	EWC_DualWeapon = 10,
	EWC_MeleeWeapon = 11,
	EWC_FlameThrower = 12,
	EWC_UGCSkill = 13,
	EWC_SP_Weapon_Melee = 100,
	EWC_SP_Weapon_Firearms = 101,
	EWC_SP_Weapon_Work = 102,
	EWC_SP_Weapon_PokemonGun = 103,
	EWC_SP_Weapon_Staff = 104,
	EWC_SP_Weapon_TalentUltimate = 105,
	EWC_SP_Weapon_Morph = 106,
	ESGWeaponCategory_MAX = 107
};

// Package: UMG
// Enums: 18

// Object: Enum UMG.ESlateAccessibleBehavior
enum class ESlateAccessibleBehavior : uint8_t
{
	NotAccessible = 0,
	Auto = 1,
	Summary = 2,
	Custom = 3,
	ToolTip = 4,
	ESlateAccessibleBehavior_MAX = 5
};

// Object: Enum UMG.ESlateVisibility
enum class ESlateVisibility : uint8_t
{
	Visible = 0,
	Collapsed = 1,
	Hidden = 2,
	HitTestInvisible = 3,
	SelfHitTestInvisible = 4,
	ESlateVisibility_MAX = 5
};

// Object: Enum UMG.EVirtualKeyboardType
enum class EVirtualKeyboardType : uint8_t
{
	Default = 0,
	Number = 1,
	Web = 2,
	Email = 3,
	Password = 4,
	AlphaNumeric = 5,
	EVirtualKeyboardType_MAX = 6
};

// Object: Enum UMG.EWidgetAnimationEvent
enum class EWidgetAnimationEvent : uint8_t
{
	Started = 0,
	Finished = 1,
	EWidgetAnimationEvent_MAX = 2
};

// Object: Enum UMG.EUMGSequencePlayMode
enum class EUMGSequencePlayMode : uint8_t
{
	Forward = 0,
	Reverse = 1,
	PingPong = 2,
	EUMGSequencePlayMode_MAX = 3
};

// Object: Enum UMG.EWidgetTickFrequency
enum class EWidgetTickFrequency : uint8_t
{
	Never = 0,
	Auto = 1,
	EWidgetTickFrequency_MAX = 2
};

// Object: Enum UMG.ESlateSizeRule
enum class ESlateSizeRule : uint8_t
{
	Automatic = 0,
	Fill = 1,
	ESlateSizeRule_MAX = 2
};

// Object: Enum UMG.EDynamicBoxType
enum class EDynamicBoxType : uint8_t
{
	Horizontal = 0,
	Vertical = 1,
	Wrap = 2,
	VerticalWrap = 3,
	Radial = 4,
	Overlay = 5,
	EDynamicBoxType_MAX = 6
};

// Object: Enum UMG.EDragPivot
enum class EDragPivot : uint8_t
{
	MouseDown = 0,
	TopLeft = 1,
	TopCenter = 2,
	TopRight = 3,
	CenterLeft = 4,
	CenterCenter = 5,
	CenterRight = 6,
	BottomLeft = 7,
	BottomCenter = 8,
	BottomRight = 9,
	EDragPivot_MAX = 10
};

// Object: Enum UMG.EWidgetDesignFlags
enum class EWidgetDesignFlags : uint8_t
{
	None = 0,
	Designing = 1,
	ShowOutline = 2,
	ExecutePreConstruct = 4,
	EWidgetDesignFlags_MAX = 5
};

// Object: Enum UMG.EBindingKind
enum class EBindingKind : uint8_t
{
	Function = 0,
	Property = 1,
	EBindingKind_MAX = 2
};

// Object: Enum UMG.ETickMode
enum class ETickMode : uint8_t
{
	Disabled = 0,
	Enabled = 1,
	Automatic = 2,
	ETickMode_MAX = 3
};

// Object: Enum UMG.EWindowVisibility
enum class EWindowVisibility : uint8_t
{
	Visible = 0,
	SelfHitTestInvisible = 1,
	EWindowVisibility_MAX = 2
};

// Object: Enum UMG.EWidgetGeometryMode
enum class EWidgetGeometryMode : uint8_t
{
	Plane = 0,
	Cylinder = 1,
	EWidgetGeometryMode_MAX = 2
};

// Object: Enum UMG.EWidgetBlendMode
enum class EWidgetBlendMode : uint8_t
{
	Opaque = 0,
	Masked = 1,
	Transparent = 2,
	EWidgetBlendMode_MAX = 3
};

// Object: Enum UMG.EWidgetTimingPolicy
enum class EWidgetTimingPolicy : uint8_t
{
	RealTime = 0,
	GameTime = 1,
	EWidgetTimingPolicy_MAX = 2
};

// Object: Enum UMG.EWidgetSpace
enum class EWidgetSpace : uint8_t
{
	World = 0,
	Screen = 1,
	EWidgetSpace_MAX = 2
};

// Object: Enum UMG.EWidgetInteractionSource
enum class EWidgetInteractionSource : uint8_t
{
	World = 0,
	Mouse = 1,
	CenterScreen = 2,
	Custom = 3,
	EWidgetInteractionSource_MAX = 4
};

// Package: MegaUI
// Enums: 5

// Object: Enum MegaUI.CurveTexTileType
enum class ECurveTexTileType : uint8_t
{
	NoTile = 0,
	Vertical = 1,
	VerticalStrentch = 2,
	CurveTexTileType_MAX = 3
};

// Object: Enum MegaUI.CurveDistortType
enum class ECurveDistortType : uint8_t
{
	None = 0,
	Middle = 1,
	ExpandPoints = 2,
	CurveDistortType_MAX = 3
};

// Object: Enum MegaUI.CurveExpandType
enum class ECurveExpandType : uint8_t
{
	VerticalTwoPoint = 0,
	Horizontal = 1,
	Vertical = 2,
	CurveExpandType_MAX = 3
};

// Object: Enum MegaUI.CurveMeshType
enum class ECurveMeshType : uint8_t
{
	Line = 0,
	Beiser = 1,
	Spline = 2,
	CustomPointsLine = 3,
	CurveMeshType_MAX = 4
};

// Object: Enum MegaUI.EOSGMaskClipType
enum class EOSGMaskClipType : uint8_t
{
	EM_None = 0,
	EM_Mask = 1,
	EM_Clip = 2,
	EM_MAX = 3
};

// Package: ParallelWorld
// Enums: 3

// Object: Enum ParallelWorld.EParallelWorldState
enum class EParallelWorldState : uint8_t
{
	Invalid = 0,
	Created = 1,
	Loaded = 2,
	Background = 3,
	Active = 4,
	Killed = 5,
	EParallelWorldState_MAX = 6
};

// Object: Enum ParallelWorld.EParallelWorldDrawMode
enum class EParallelWorldDrawMode : uint8_t
{
	RenderTargetAsFramebuffer = 0,
	RenderTargetAsPixels = 1,
	RenderTargetAsNativeWindow = 2,
	RenderTargetAsNothing = 3,
	EParallelWorldDrawMode_MAX = 4
};

// Object: Enum ParallelWorld.EParallelWorldManagerState
enum class EParallelWorldManagerState : uint8_t
{
	Uninitialized = 0,
	Initializing = 1,
	Initialized = 2,
	Deinitializing = 3,
	Deinitialized = 4,
	EParallelWorldManagerState_MAX = 5
};

// Package: ApexDestruction
// Enums: 1

// Object: Enum ApexDestruction.EImpactDamageOverride
enum class EImpactDamageOverride : uint8_t
{
	IDO_None = 0,
	IDO_On = 1,
	IDO_Off = 2,
	IDO_MAX = 3
};

// Package: HorizonTween
// Enums: 6

// Object: Enum HorizonTween.EHorizonTweenPlayMode
enum class EHorizonTweenPlayMode : uint8_t
{
	Forward = 0,
	Reverse = 1,
	PingPong = 2,
	EHorizonTweenPlayMode_MAX = 3
};

// Object: Enum HorizonTween.EHorizonTweenLerpMode
enum class EHorizonTweenLerpMode : uint8_t
{
	Lerp = 0,
	LerpStable = 1,
	InterpSinIn = 2,
	InterpSinOut = 3,
	InterpSinInOut = 4,
	InterpExpoIn = 5,
	InterpExpoOut = 6,
	InterpExpoInOut = 7,
	InterpCircularIn = 8,
	InterpCircularOut = 9,
	InterpCircularInOut = 10,
	InterpEaseIn = 11,
	InterpEaseOut = 12,
	InterpEaseInOut = 13,
	BounceIn = 14,
	BounceOut = 15,
	BounceInOut = 16,
	ElasticIn = 17,
	ElasticOut = 18,
	ElasticInOut = 19,
	BackIn = 20,
	BackOut = 21,
	BackInOut = 22,
	QuadIn = 23,
	QuadOut = 24,
	QuadInOut = 25,
	CubicIn = 26,
	CubicOut = 27,
	CubicInOut = 28,
	QuartIn = 29,
	QuartOut = 30,
	QuartInOut = 31,
	QuintIn = 32,
	QuintOut = 33,
	QuintInOut = 34,
	EHorizonTweenLerpMode_MAX = 35
};

// Object: Enum HorizonTween.EHorizonTweenCoordinateSpace
enum class EHorizonTweenCoordinateSpace : uint8_t
{
	Local = 0,
	World = 1,
	EHorizonTweenCoordinateSpace_MAX = 2
};

// Object: Enum HorizonTween.EHorizonTweenVectorConstraints
enum class EHorizonTweenVectorConstraints : uint8_t
{
	None = 0,
	XOnly = 1,
	YOnly = 2,
	ZOnly = 3,
	XYPlane = 4,
	YZPlane = 5,
	XZPlane = 6,
	EHorizonTweenVectorConstraints_MAX = 7
};

// Object: Enum HorizonTween.EHorizonTweenRotatorConstraints
enum class EHorizonTweenRotatorConstraints : uint8_t
{
	None = 0,
	RollOnly = 1,
	PitchOnly = 2,
	YawOnly = 3,
	RollAndPitchOnly = 4,
	PitchAndYawOnly = 5,
	RollAndYawOnly = 6,
	EHorizonTweenRotatorConstraints_MAX = 7
};

// Object: Enum HorizonTween.EHorizonTweenVector2DConstraints
enum class EHorizonTweenVector2DConstraints : uint8_t
{
	None = 0,
	XOnly = 1,
	YOnly = 2,
	EHorizonTweenVector2DConstraints_MAX = 3
};

// Package: SlateCore
// Enums: 37

// Object: Enum SlateCore.EUINavigation
enum class EUINavigation : uint8_t
{
	Left = 0,
	Right = 1,
	Up = 2,
	Down = 3,
	Next = 4,
	Previous = 5,
	Num = 6,
	Invalid = 7,
	EUINavigation_MAX = 8
};

// Object: Enum SlateCore.ECheckBoxState
enum class ECheckBoxState : uint8_t
{
	Unchecked = 0,
	Checked = 1,
	Undetermined = 2,
	ECheckBoxState_MAX = 3
};

// Object: Enum SlateCore.EWidgetClipping
enum class EWidgetClipping : uint8_t
{
	Inherit = 0,
	ClipToBounds = 1,
	ClipToBoundsWithoutIntersecting = 2,
	ClipToBoundsAlways = 3,
	OnDemand = 4,
	EWidgetClipping_MAX = 5
};

// Object: Enum SlateCore.ESlateBrushImageType
enum class ESlateBrushImageType : uint8_t
{
	NoImage = 0,
	FullColor = 1,
	Linear = 2,
	ESlateBrushImageType_MAX = 3
};

// Object: Enum SlateCore.ESlateBrushMirrorType
enum class ESlateBrushMirrorType : uint8_t
{
	NoMirror = 0,
	Horizontal = 1,
	Vertical = 2,
	Both = 3,
	ESlateBrushMirrorType_MAX = 4
};

// Object: Enum SlateCore.ESlateBrushTileType
enum class ESlateBrushTileType : uint8_t
{
	NoTile = 0,
	Horizontal = 1,
	Vertical = 2,
	Both = 3,
	ESlateBrushTileType_MAX = 4
};

// Object: Enum SlateCore.ESlateBrushDrawType
enum class ESlateBrushDrawType : uint8_t
{
	NoDrawType = 0,
	Box = 1,
	Border = 2,
	Image = 3,
	DeformedBox = 4,
	ESlateBrushDrawType_MAX = 5
};

// Object: Enum SlateCore.ESlateColorStylingMode
enum class ESlateColorStylingMode : uint8_t
{
	UseColor_Specified = 0,
	UseColor_Specified_Link = 1,
	UseColor_Foreground = 2,
	UseColor_Foreground_Subdued = 3,
	UseColor_MAX = 4
};

// Object: Enum SlateCore.EUINavigationRule
enum class EUINavigationRule : uint8_t
{
	Escape = 0,
	Explicit = 1,
	Wrap = 2,
	Stop = 3,
	Custom = 4,
	CustomBoundary = 5,
	Invalid = 6,
	EUINavigationRule_MAX = 7
};

// Object: Enum SlateCore.EFlowDirectionPreference
enum class EFlowDirectionPreference : uint8_t
{
	Inherit = 0,
	Culture = 1,
	LeftToRight = 2,
	RightToLeft = 3,
	EFlowDirectionPreference_MAX = 4
};

// Object: Enum SlateCore.EColorVisionDeficiency
enum class EColorVisionDeficiency : uint8_t
{
	NormalVision = 0,
	Deuteranope = 1,
	Protanope = 2,
	Tritanope = 3,
	EColorVisionDeficiency_MAX = 4
};

// Object: Enum SlateCore.ESelectInfo
enum class ESelectInfo : uint8_t
{
	OnKeyPress = 0,
	OnNavigation = 1,
	OnMouseClick = 2,
	Direct = 3,
	ESelectInfo_MAX = 4
};

// Object: Enum SlateCore.ETextCommit
enum class ETextCommit : uint8_t
{
	Default = 0,
	OnEnter = 1,
	OnUserMovedFocus = 2,
	OnCleared = 3,
	ETextCommit_MAX = 4
};

// Object: Enum SlateCore.ETextShapingMethod
enum class ETextShapingMethod : uint8_t
{
	Auto = 0,
	KerningOnly = 1,
	FullShaping = 2,
	ETextShapingMethod_MAX = 3
};

// Object: Enum SlateCore.EMenuPlacement
enum class EMenuPlacement : uint8_t
{
	MenuPlacement_BelowAnchor = 0,
	MenuPlacement_CenteredBelowAnchor = 1,
	MenuPlacement_BelowRightAnchor = 2,
	MenuPlacement_ComboBox = 3,
	MenuPlacement_ComboBoxRight = 4,
	MenuPlacement_MenuRight = 5,
	MenuPlacement_AboveAnchor = 6,
	MenuPlacement_CenteredAboveAnchor = 7,
	MenuPlacement_AboveRightAnchor = 8,
	MenuPlacement_MenuLeft = 9,
	MenuPlacement_Center = 10,
	MenuPlacement_RightLeftCenter = 11,
	MenuPlacement_MatchBottomLeft = 12,
	MenuPlacement_MAX = 13
};

// Object: Enum SlateCore.EVerticalAlignment
enum class EVerticalAlignment : uint8_t
{
	VAlign_Fill = 0,
	VAlign_Top = 1,
	VAlign_Center = 2,
	VAlign_Bottom = 3,
	VAlign_MAX = 4
};

// Object: Enum SlateCore.EHorizontalAlignment
enum class EHorizontalAlignment : uint8_t
{
	HAlign_Fill = 0,
	HAlign_Left = 1,
	HAlign_Center = 2,
	HAlign_Right = 3,
	HAlign_MAX = 4
};

// Object: Enum SlateCore.EFontLayoutMethod
enum class EFontLayoutMethod : uint8_t
{
	Metrics = 0,
	BoundingBox = 1,
	EFontLayoutMethod_MAX = 2
};

// Object: Enum SlateCore.EFontLoadingPolicy
enum class EFontLoadingPolicy : uint8_t
{
	LazyLoad = 0,
	Stream = 1,
	Inline = 2,
	EFontLoadingPolicy_MAX = 3
};

// Object: Enum SlateCore.EFontHinting
enum class EFontHinting : uint8_t
{
	Default = 0,
	Auto = 1,
	AutoLight = 2,
	Monochrome = 3,
	None = 4,
	EFontHinting_MAX = 5
};

// Object: Enum SlateCore.EFocusCause
enum class EFocusCause : uint8_t
{
	Mouse = 0,
	Navigation = 1,
	SetDirectly = 2,
	Cleared = 3,
	OtherWidgetLostFocus = 4,
	WindowActivate = 5,
	EFocusCause_MAX = 6
};

// Object: Enum SlateCore.EGuideMode
enum class EGuideMode : uint8_t
{
	EGM_Normal = 0,
	EGM_Circle = 1,
	EGM_Count = 2,
	EGM_MAX = 3
};

// Object: Enum SlateCore.ESlateDebuggingFocusEvent
enum class ESlateDebuggingFocusEvent : uint8_t
{
	FocusChanging = 0,
	FocusLost = 1,
	FocusReceived = 2,
	MAX = 3
};

// Object: Enum SlateCore.ESlateDebuggingNavigationMethod
enum class ESlateDebuggingNavigationMethod : uint8_t
{
	Unknown = 0,
	Explicit = 1,
	CustomDelegateBound = 2,
	CustomDelegateUnbound = 3,
	NextOrPrevious = 4,
	HitTestGrid = 5,
	ESlateDebuggingNavigationMethod_MAX = 6
};

// Object: Enum SlateCore.ESlateDebuggingStateChangeEvent
enum class ESlateDebuggingStateChangeEvent : uint8_t
{
	MouseCaptureGained = 0,
	MouseCaptureLost = 1,
	ESlateDebuggingStateChangeEvent_MAX = 2
};

// Object: Enum SlateCore.ESlateDebuggingInputEvent
enum class ESlateDebuggingInputEvent : uint8_t
{
	MouseMove = 0,
	MouseEnter = 1,
	MouseLeave = 2,
	PreviewMouseButtonDown = 3,
	MouseButtonDown = 4,
	MouseButtonUp = 5,
	MouseButtonDoubleClick = 6,
	MouseWheel = 7,
	TouchStart = 8,
	TouchEnd = 9,
	TouchForceChanged = 10,
	TouchFirstMove = 11,
	TouchMoved = 12,
	DragDetected = 13,
	DragEnter = 14,
	DragLeave = 15,
	DragOver = 16,
	DragDrop = 17,
	DropMessage = 18,
	PreviewKeyDown = 19,
	KeyDown = 20,
	KeyUp = 21,
	KeyChar = 22,
	AnalogInput = 23,
	TouchGesture = 24,
	MotionDetected = 25,
	MAX = 26
};

// Object: Enum SlateCore.EScrollDirection
enum class EScrollDirection : uint8_t
{
	Scroll_Down = 0,
	Scroll_Up = 1,
	Scroll_MAX = 2
};

// Object: Enum SlateCore.EOrientation
enum class EOrientation : uint8_t
{
	Orient_Horizontal = 0,
	Orient_Vertical = 1,
	Orient_MAX = 2
};

// Object: Enum SlateCore.ENavigationGenesis
enum class ENavigationGenesis : uint8_t
{
	Keyboard = 0,
	Controller = 1,
	User = 2,
	ENavigationGenesis_MAX = 3
};

// Object: Enum SlateCore.ENavigationSource
enum class ENavigationSource : uint8_t
{
	FocusedWidget = 0,
	WidgetUnderCursor = 1,
	ENavigationSource_MAX = 2
};

// Object: Enum SlateCore.EUINavigationAction
enum class EUINavigationAction : uint8_t
{
	Accept = 0,
	Back = 1,
	Num = 2,
	Invalid = 3,
	EUINavigationAction_MAX = 4
};

// Object: Enum SlateCore.EButtonPressMethod
enum class EButtonPressMethod : uint8_t
{
	DownAndUp = 0,
	ButtonPress = 1,
	ButtonRelease = 2,
	EButtonPressMethod_MAX = 3
};

// Object: Enum SlateCore.EButtonTouchMethod
enum class EButtonTouchMethod : uint8_t
{
	DownAndUp = 0,
	Down = 1,
	PreciseTap = 2,
	EButtonTouchMethod_MAX = 3
};

// Object: Enum SlateCore.EButtonClickMethod
enum class EButtonClickMethod : uint8_t
{
	DownAndUp = 0,
	MouseDown = 1,
	MouseUp = 2,
	PreciseClick = 3,
	EButtonClickMethod_MAX = 4
};

// Object: Enum SlateCore.ESlateCheckBoxType
enum class ESlateCheckBoxType : uint8_t
{
	CheckBox = 0,
	ToggleButton = 1,
	ESlateCheckBoxType_MAX = 2
};

// Object: Enum SlateCore.ESlateParentWindowSearchMethod
enum class ESlateParentWindowSearchMethod : uint8_t
{
	ActiveWindow = 0,
	MainWindow = 1,
	ESlateParentWindowSearchMethod_MAX = 2
};

// Object: Enum SlateCore.EConsumeMouseWheel
enum class EConsumeMouseWheel : uint8_t
{
	WhenScrollingPossible = 0,
	Always = 1,
	Never = 2,
	EConsumeMouseWheel_MAX = 3
};

// Package: Paper2D
// Enums: 7

// Object: Enum Paper2D.EFlipbookCollisionMode
enum class EFlipbookCollisionMode : uint8_t
{
	NoCollision = 0,
	FirstFrameCollision = 1,
	EachFrameCollision = 2,
	EFlipbookCollisionMode_MAX = 3
};

// Object: Enum Paper2D.EPaperSpriteAtlasPadding
enum class EPaperSpriteAtlasPadding : uint8_t
{
	DilateBorder = 0,
	PadWithZero = 1,
	EPaperSpriteAtlasPadding_MAX = 2
};

// Object: Enum Paper2D.ETileMapProjectionMode
enum class ETileMapProjectionMode : uint8_t
{
	Orthogonal = 0,
	IsometricDiamond = 1,
	IsometricStaggered = 2,
	HexagonalStaggered = 3,
	ETileMapProjectionMode_MAX = 4
};

// Object: Enum Paper2D.ESpritePivotMode
enum class ESpritePivotMode : uint8_t
{
	Top_Left = 0,
	Top_Center = 1,
	Top_Right = 2,
	Center_Left = 3,
	Center_Center = 4,
	Center_Right = 5,
	Bottom_Left = 6,
	Bottom_Center = 7,
	Bottom_Right = 8,
	Custom = 9,
	ESpritePivotMode_MAX = 10
};

// Object: Enum Paper2D.ESpritePolygonMode
enum class ESpritePolygonMode : uint8_t
{
	SourceBoundingBox = 0,
	TightBoundingBox = 1,
	ShrinkWrapped = 2,
	FullyCustom = 3,
	Diced = 4,
	ESpritePolygonMode_MAX = 5
};

// Object: Enum Paper2D.ESpriteShapeType
enum class ESpriteShapeType : uint8_t
{
	Box = 0,
	Circle = 1,
	Polygon = 2,
	ESpriteShapeType_MAX = 3
};

// Object: Enum Paper2D.ESpriteCollisionMode
enum class ESpriteCollisionMode : uint8_t
{
	None = 0,
	Use2DPhysics = 1,
	Use3DPhysics = 2,
	ESpriteCollisionMode_MAX = 3
};

// Package: UIParticle
// Enums: 5

// Object: Enum UIParticle.EUIParticlePropertyType
enum class EUIParticlePropertyType : uint8_t
{
	Float = 0,
	FloatRange = 1,
	FloatCurve = 2,
	FloatCurveRange = 3,
	Vector2D = 4,
	Vector2DRange = 5,
	LinearColorCurve = 6,
	LinearColorCurveRange = 7,
	EUIParticlePropertyType_MAX = 8
};

// Object: Enum UIParticle.EParticleDrawEffect
enum class EParticleDrawEffect : uint8_t
{
	None = 0,
	NoBlending = 1,
	PreMultipliedAlpha = 2,
	NoGamma = 4,
	InvertAlpha = 8,
	NoPixelSnapping = 16,
	DisabledEffect = 32,
	IgnoreTextureAlpha = 64,
	ReverseGamma = 128,
	EParticleDrawEffect_MAX = 129
};

// Object: Enum UIParticle.EPositionType
enum class EPositionType : uint8_t
{
	FREE = 0,
	RELATIVE = 1,
	EPositionType_MAX = 2
};

// Object: Enum UIParticle.EEmitterType
enum class EEmitterType : uint8_t
{
	Gravity = 0,
	Radial = 1,
	Curve = 2,
	EEmitterType_MAX = 3
};

// Object: Enum UIParticle.ECurveType
enum class ECurveType : uint8_t
{
	ParticleLifePercent = 0,
	ParticleLifeTime = 1,
	EmitLifeTime = 2,
	ECurveType_MAX = 3
};

// Package: MoeGPM
// Enums: 1

// Object: Enum MoeGPM.EMoeApmInfoType
enum class EMoeApmInfoType : uint8_t
{
	Unknown = 0,
	Pss = 1,
	CurrentNow = 2,
	Voltage = 3,
	Apm_Status = 4,
	BatteryTemperature = 5,
	CpuTemperature = 6,
	IOSThermalState = 7,
	WifiFreqEnable5G = 8,
	MoeSwap = 9,
	MonoReserved = 10,
	CpuUsage = 11,
	NetState = 12,
	VpnState = 13,
	GPURenderer = 14,
	GPUDevice = 15,
	GPUTiler = 16,
	EMoeApmInfoType_MAX = 17
};

// Package: UGC
// Enums: 18

// Object: Enum UGC.EChessAndCardsType
enum class EChessAndCardsType : uint8_t
{
	Mahjong = 1,
	Poker = 2,
	EChessAndCardsType_MAX = 3
};

// Object: Enum UGC.EViewCubeDirection
enum class EViewCubeDirection : uint8_t
{
	VC_None = 0,
	VC_Up = 1,
	VC_Down = 2,
	VC_Forward = 3,
	VC_Backward = 4,
	VC_Left = 5,
	VC_Right = 6,
	VC_MAX = 7
};

// Object: Enum UGC.EUGCCOSHandlerStatus
enum class EUGCCOSHandlerStatus : uint8_t
{
	Free = 0,
	Initialized = 1,
	HttpProcessing = 2,
	HttpFailed = 3,
	HttpSucceed = 4,
	EUGCCOSHandlerStatus_MAX = 5
};

// Object: Enum UGC.EOutlineType
enum class EOutlineType : uint8_t
{
	MeshNormal = 0,
	MeshNormal_PostProcess = 1,
	PostProcess = 2,
	EOutlineType_MAX = 3
};

// Object: Enum UGC.EGirdType
enum class EGirdType : uint8_t
{
	Line = 0,
	EGirdType_MAX = 1
};

// Object: Enum UGC.EUGCTryPlayType
enum class EUGCTryPlayType : uint8_t
{
	ETPT_Normal = 0,
	ETPT_Publish = 1,
	ETPT_UGCP = 2,
	ETPT_UGCLobbyTest = 3,
	ETPT_MAX = 4
};

// Object: Enum UGC.EUGCFixedZScaleType
enum class EUGCFixedZScaleType : uint8_t
{
	None = 0,
	Mid = 1,
	Up = 2,
	Bottom = 3,
	EUGCFixedZScaleType_MAX = 4
};

// Object: Enum UGC.EUGCFixedXScaleType
enum class EUGCFixedXScaleType : uint8_t
{
	None = 0,
	Mid = 1,
	Forward = 2,
	Back = 3,
	EUGCFixedXScaleType_MAX = 4
};

// Object: Enum UGC.EUGCFixedYScaleType
enum class EUGCFixedYScaleType : uint8_t
{
	None = 0,
	Mid = 1,
	Left = 2,
	Right = 3,
	EUGCFixedYScaleType_MAX = 4
};

// Object: Enum UGC.EUGCLocomotorTemplateType
enum class EUGCLocomotorTemplateType : uint8_t
{
	Custom = 0,
	OnewayMove = 1,
	LoopAndBackMove = 2,
	SimpleRotation = 3,
	Swing = 4,
	WayPoint = 5,
	OnewayScale = 6,
	EUGCLocomotorTemplateType_MAX = 7
};

// Object: Enum UGC.EUGCLocomotorMoveLoopType
enum class EUGCLocomotorMoveLoopType : uint8_t
{
	Custom = 0,
	Oneway = 1,
	Loop = 2,
	LoopAndBack = 3,
	RoundTrip = 4,
	EUGCLocomotorMoveLoopType_MAX = 5
};

// Object: Enum UGC.EUGCGameEditState
enum class EUGCGameEditState : uint8_t
{
	Undefine = 0,
	Edit = 1,
	Play = 2,
	Photo = 3,
	EUGCGameEditState_MAX = 4
};

// Object: Enum UGC.EActorAIColorType
enum class EActorAIColorType : uint8_t
{
	NoMask = 0,
	Masked = 1,
	Single = 2,
	Multi = 3,
	Transparent = 4,
	EActorAIColorType_MAX = 5
};

// Object: Enum UGC.EUGCPreviewNavMeshOperation
enum class EUGCPreviewNavMeshOperation : uint8_t
{
	Generate = 0,
	GenerateAndShow = 1,
	Show = 2,
	Hide = 3,
	Update = 4,
	Dispose = 5,
	EUGCPreviewNavMeshOperation_MAX = 6
};

// Object: Enum UGC.FUGCParticleVectorTimeParam
enum class EFUGCParticleVectorTimeParam : uint8_t
{
	X = 0,
	Y = 1,
	Z = 2,
	Count = 3,
	FUGCParticleVectorTimeParam_MAX = 4
};

// Object: Enum UGC.EParamType
enum class EParamType : uint8_t
{
	ParamVector = 0,
	ParamFloat = 1,
	EParamType_MAX = 2
};

// Object: Enum UGC.EParamSetterType
enum class EParamSetterType : uint8_t
{
	Vector = 0,
	float = 1,
	EParamSetterType_MAX = 2
};

// Object: Enum UGC.EUGCPathFindType
enum class EUGCPathFindType : uint8_t
{
	Default = 0,
	IgnoreObstacles = 1,
	EUGCPathFindType_MAX = 2
};

// Package: AIModule
// Enums: 51

// Object: Enum AIModule.EPathFollowingResult
enum class EPathFollowingResult : uint8_t
{
	Success = 0,
	Blocked = 1,
	OffPath = 2,
	Aborted = 3,
	Skipped_DEPRECATED = 4,
	Invalid = 5,
	EPathFollowingResult_MAX = 6
};

// Object: Enum AIModule.EEnvQueryStatus
enum class EEnvQueryStatus : uint8_t
{
	Processing = 0,
	Success = 1,
	Failed = 2,
	Aborted = 3,
	OwnerLost = 4,
	MissingParam = 5,
	EEnvQueryStatus_MAX = 6
};

// Object: Enum AIModule.EAISenseNotifyType
enum class EAISenseNotifyType : uint8_t
{
	OnEveryPerception = 0,
	OnPerceptionChange = 1,
	EAISenseNotifyType_MAX = 2
};

// Object: Enum AIModule.EAITaskPriority
enum class EAITaskPriority : uint8_t
{
	Lowest = 0,
	Low = 64,
	AutonomousAI = 127,
	High = 192,
	Ultimate = 254,
	EAITaskPriority_MAX = 255
};

// Object: Enum AIModule.EGenericAICheck
enum class EGenericAICheck : uint8_t
{
	Less = 0,
	LessOrEqual = 1,
	Equal = 2,
	NotEqual = 3,
	GreaterOrEqual = 4,
	Greater = 5,
	IsTrue = 6,
	MAX = 7
};

// Object: Enum AIModule.EAILockSource
enum class EAILockSource : uint8_t
{
	Animation = 0,
	Logic = 1,
	Script = 2,
	Gameplay = 3,
	MAX = 4
};

// Object: Enum AIModule.EAIRequestPriority
enum class EAIRequestPriority : uint8_t
{
	SoftScript = 0,
	Logic = 1,
	HardScript = 2,
	Reaction = 3,
	Ultimate = 4,
	MAX = 5
};

// Object: Enum AIModule.EPawnActionEventType
enum class EPawnActionEventType : uint8_t
{
	Invalid = 0,
	FailedToStart = 1,
	InstantAbort = 2,
	FinishedAborting = 3,
	FinishedExecution = 4,
	Push = 5,
	EPawnActionEventType_MAX = 6
};

// Object: Enum AIModule.EPawnActionResult
enum class EPawnActionResult : uint8_t
{
	NotStarted = 0,
	InProgress = 1,
	Success = 2,
	Failed = 3,
	Aborted = 4,
	EPawnActionResult_MAX = 5
};

// Object: Enum AIModule.EPawnActionAbortState
enum class EPawnActionAbortState : uint8_t
{
	NeverStarted = 0,
	NotBeingAborted = 1,
	MarkPendingAbort = 2,
	LatentAbortInProgress = 3,
	AbortDone = 4,
	MAX = 5
};

// Object: Enum AIModule.FAIDistanceType
enum class EFAIDistanceType : uint8_t
{
	Distance3D = 0,
	Distance2D = 1,
	DistanceZ = 2,
	MAX = 3
};

// Object: Enum AIModule.EAIOptionFlag
enum class EAIOptionFlag : uint8_t
{
	Default = 0,
	Enable = 1,
	Disable = 2,
	MAX = 3
};

// Object: Enum AIModule.EBTFlowAbortMode
enum class EBTFlowAbortMode : uint8_t
{
	None = 0,
	LowerPriority = 1,
	Self = 2,
	Both = 3,
	EBTFlowAbortMode_MAX = 4
};

// Object: Enum AIModule.EBTNodeResult
enum class EBTNodeResult : uint8_t
{
	Succeeded = 0,
	Failed = 1,
	Aborted = 2,
	InProgress = 3,
	EBTNodeResult_MAX = 4
};

// Object: Enum AIModule.ETextKeyOperation
enum class ETextKeyOperation : uint8_t
{
	Equal = 0,
	NotEqual = 1,
	Contain = 2,
	NotContain = 3,
	ETextKeyOperation_MAX = 4
};

// Object: Enum AIModule.EArithmeticKeyOperation
enum class EArithmeticKeyOperation : uint8_t
{
	Equal = 0,
	NotEqual = 1,
	Less = 2,
	LessOrEqual = 3,
	Greater = 4,
	GreaterOrEqual = 5,
	EArithmeticKeyOperation_MAX = 6
};

// Object: Enum AIModule.EBasicKeyOperation
enum class EBasicKeyOperation : uint8_t
{
	Set = 0,
	NotSet = 1,
	EBasicKeyOperation_MAX = 2
};

// Object: Enum AIModule.EBTParallelMode
enum class EBTParallelMode : uint8_t
{
	AbortBackground = 0,
	WaitForBackground = 1,
	EBTParallelMode_MAX = 2
};

// Object: Enum AIModule.EBTDecoratorLogic
enum class EBTDecoratorLogic : uint8_t
{
	Invalid = 0,
	Test = 1,
	And = 2,
	Or = 3,
	Not = 4,
	EBTDecoratorLogic_MAX = 5
};

// Object: Enum AIModule.EBTChildIndex
enum class EBTChildIndex : uint8_t
{
	FirstNode = 0,
	TaskNode = 1,
	EBTChildIndex_MAX = 2
};

// Object: Enum AIModule.EBTBlackboardRestart
enum class EBTBlackboardRestart : uint8_t
{
	ValueChange = 0,
	ResultChange = 1,
	EBTBlackboardRestart_MAX = 2
};

// Object: Enum AIModule.EBlackBoardEntryComparison
enum class EBlackBoardEntryComparison : uint8_t
{
	Equal = 0,
	NotEqual = 1,
	EBlackBoardEntryComparison_MAX = 2
};

// Object: Enum AIModule.EPathExistanceQueryType
enum class EPathExistanceQueryType : uint8_t
{
	NavmeshRaycast2D = 0,
	HierarchicalQuery = 1,
	RegularPathFinding = 2,
	EPathExistanceQueryType_MAX = 3
};

// Object: Enum AIModule.EPointOnCircleSpacingMethod
enum class EPointOnCircleSpacingMethod : uint8_t
{
	BySpaceBetween = 0,
	ByNumberOfPoints = 1,
	EPointOnCircleSpacingMethod_MAX = 2
};

// Object: Enum AIModule.EEQSNormalizationType
enum class EEQSNormalizationType : uint8_t
{
	Absolute = 0,
	RelativeToScores = 1,
	EEQSNormalizationType_MAX = 2
};

// Object: Enum AIModule.EEnvTestDistance
enum class EEnvTestDistance : uint8_t
{
	Distance3D = 0,
	Distance2D = 1,
	DistanceZ = 2,
	DistanceAbsoluteZ = 3,
	EEnvTestDistance_MAX = 4
};

// Object: Enum AIModule.EEnvTestDot
enum class EEnvTestDot : uint8_t
{
	Dot3D = 0,
	Dot2D = 1,
	EEnvTestDot_MAX = 2
};

// Object: Enum AIModule.EEnvTestPathfinding
enum class EEnvTestPathfinding : uint8_t
{
	PathExist = 0,
	PathCost = 1,
	PathLength = 2,
	EEnvTestPathfinding_MAX = 3
};

// Object: Enum AIModule.EEnvQueryTestClamping
enum class EEnvQueryTestClamping : uint8_t
{
	None = 0,
	SpecifiedValue = 1,
	FilterThreshold = 2,
	EEnvQueryTestClamping_MAX = 3
};

// Object: Enum AIModule.EEnvDirection
enum class EEnvDirection : uint8_t
{
	TwoPoints = 0,
	Rotation = 1,
	EEnvDirection_MAX = 2
};

// Object: Enum AIModule.EEnvOverlapShape
enum class EEnvOverlapShape : uint8_t
{
	Box = 0,
	Sphere = 1,
	Capsule = 2,
	EEnvOverlapShape_MAX = 3
};

// Object: Enum AIModule.EEnvTraceShape
enum class EEnvTraceShape : uint8_t
{
	Line = 0,
	Box = 1,
	Sphere = 2,
	Capsule = 3,
	EEnvTraceShape_MAX = 4
};

// Object: Enum AIModule.EEnvQueryTrace
enum class EEnvQueryTrace : uint8_t
{
	None = 0,
	Navigation = 1,
	Geometry = 2,
	NavigationOverLedges = 3,
	EEnvQueryTrace_MAX = 4
};

// Object: Enum AIModule.EAIParamType
enum class EAIParamType : uint8_t
{
	Float = 0,
	Int = 1,
	Bool = 2,
	MAX = 3
};

// Object: Enum AIModule.EEnvQueryParam
enum class EEnvQueryParam : uint8_t
{
	Float = 0,
	Int = 1,
	Bool = 2,
	EEnvQueryParam_MAX = 3
};

// Object: Enum AIModule.EEnvQueryRunMode
enum class EEnvQueryRunMode : uint8_t
{
	SingleResult = 0,
	RandomBest5Pct = 1,
	RandomBest25Pct = 2,
	AllMatching = 3,
	EEnvQueryRunMode_MAX = 4
};

// Object: Enum AIModule.EEnvTestScoreOperator
enum class EEnvTestScoreOperator : uint8_t
{
	AverageScore = 0,
	MinScore = 1,
	MaxScore = 2,
	Multiply = 3,
	EEnvTestScoreOperator_MAX = 4
};

// Object: Enum AIModule.EEnvTestFilterOperator
enum class EEnvTestFilterOperator : uint8_t
{
	AllPass = 0,
	AnyPass = 1,
	EEnvTestFilterOperator_MAX = 2
};

// Object: Enum AIModule.EEnvTestCost
enum class EEnvTestCost : uint8_t
{
	Low = 0,
	Medium = 1,
	High = 2,
	EEnvTestCost_MAX = 3
};

// Object: Enum AIModule.EEnvTestWeight
enum class EEnvTestWeight : uint8_t
{
	None = 0,
	Square = 1,
	Inverse = 2,
	Unused = 3,
	Constant = 4,
	Skip = 5,
	EEnvTestWeight_MAX = 6
};

// Object: Enum AIModule.EEnvTestScoreEquation
enum class EEnvTestScoreEquation : uint8_t
{
	Linear = 0,
	Square = 1,
	InverseLinear = 2,
	SquareRoot = 3,
	Constant = 4,
	EEnvTestScoreEquation_MAX = 5
};

// Object: Enum AIModule.EEnvTestFilterType
enum class EEnvTestFilterType : uint8_t
{
	Minimum = 0,
	Maximum = 1,
	Range = 2,
	Match = 3,
	EEnvTestFilterType_MAX = 4
};

// Object: Enum AIModule.EEnvTestPurpose
enum class EEnvTestPurpose : uint8_t
{
	Filter = 0,
	Score = 1,
	FilterAndScore = 2,
	EEnvTestPurpose_MAX = 3
};

// Object: Enum AIModule.EEnvQueryHightlightMode
enum class EEnvQueryHightlightMode : uint8_t
{
	All = 0,
	Best5Pct = 1,
	Best25Pct = 2,
	EEnvQueryHightlightMode_MAX = 3
};

// Object: Enum AIModule.ETeamAttitude
enum class ETeamAttitude : uint8_t
{
	Friendly = 0,
	Neutral = 1,
	Hostile = 2,
	ETeamAttitude_MAX = 3
};

// Object: Enum AIModule.EPathFollowingRequestResult
enum class EPathFollowingRequestResult : uint8_t
{
	Failed = 0,
	AlreadyAtGoal = 1,
	RequestSuccessful = 2,
	EPathFollowingRequestResult_MAX = 3
};

// Object: Enum AIModule.EPathFollowingAction
enum class EPathFollowingAction : uint8_t
{
	Error = 0,
	NoMove = 1,
	DirectMove = 2,
	PartialPath = 3,
	PathToGoal = 4,
	EPathFollowingAction_MAX = 5
};

// Object: Enum AIModule.EPathFollowingStatus
enum class EPathFollowingStatus : uint8_t
{
	Idle = 0,
	Waiting = 1,
	Paused = 2,
	Moving = 3,
	EPathFollowingStatus_MAX = 4
};

// Object: Enum AIModule.EPawnActionFailHandling
enum class EPawnActionFailHandling : uint8_t
{
	RequireSuccess = 0,
	IgnoreFailure = 1,
	EPawnActionFailHandling_MAX = 2
};

// Object: Enum AIModule.EPawnSubActionTriggeringPolicy
enum class EPawnSubActionTriggeringPolicy : uint8_t
{
	CopyBeforeTriggering = 0,
	ReuseInstances = 1,
	EPawnSubActionTriggeringPolicy_MAX = 2
};

// Object: Enum AIModule.EPawnActionMoveMode
enum class EPawnActionMoveMode : uint8_t
{
	UsePathfinding = 0,
	StraightLine = 1,
	EPawnActionMoveMode_MAX = 2
};

// Package: NavigationSystem
// Enums: 4

// Object: Enum NavigationSystem.ERuntimeGenerationType
enum class ERuntimeGenerationType : uint8_t
{
	Static = 0,
	DynamicModifiersOnly = 1,
	Dynamic = 2,
	LegacyGeneration = 3,
	ERuntimeGenerationType_MAX = 4
};

// Object: Enum NavigationSystem.ENavCostDisplay
enum class ENavCostDisplay : uint8_t
{
	TotalCost = 0,
	HeuristicOnly = 1,
	RealCostOnly = 2,
	ENavCostDisplay_MAX = 3
};

// Object: Enum NavigationSystem.ENavSystemOverridePolicy
enum class ENavSystemOverridePolicy : uint8_t
{
	Override = 0,
	Append = 1,
	Skip = 2,
	ENavSystemOverridePolicy_MAX = 3
};

// Object: Enum NavigationSystem.ERecastPartitioning
enum class ERecastPartitioning : uint8_t
{
	Monotone = 0,
	Watershed = 1,
	ChunkyMonotone = 2,
	ERecastPartitioning_MAX = 3
};

// Package: UGCProgram
// Enums: 2

// Object: Enum UGCProgram.EUGCPValType
enum class EUGCPValType : uint8_t
{
	UNKNOWN = 0,
	INT32 = 1,
	UINT32 = 2,
	INT64 = 3,
	UINT64 = 4,
	FLOAT = 5,
	DOUBLE = 6,
	BOOLEAN = 7,
	STRING = 8,
	VECTOR2 = 9,
	VECTOR = 10,
	VECTOR4 = 11,
	ROTATOR = 12,
	MAX = 127
};

// Object: Enum UGCProgram.EUGCProgramMoveDirection
enum class EUGCProgramMoveDirection : uint8_t
{
	Forward = 0,
	Backward = 1,
	Left = 2,
	Right = 3,
	EUGCProgramMoveDirection_MAX = 4
};

// Package: UGCExtend
// Enums: 6

// Object: Enum UGCExtend.EUGCDropItemState
enum class EUGCDropItemState : uint8_t
{
	Unknown = 0,
	EDeactivated = 1,
	EPicked = 2,
	EActivated = 3,
	EActiveInteract = 4,
	EUGCDropItemState_MAX = 5
};

// Object: Enum UGCExtend.EUGCArmorType
enum class EUGCArmorType : uint8_t
{
	Helmet = 0,
	Suit = 1,
	EUGCArmorType_MAX = 2
};

// Object: Enum UGCExtend.EUGCArmorLevel
enum class EUGCArmorLevel : uint8_t
{
	None = 0,
	LevelOne = 1,
	LevelTwo = 2,
	LevelThree = 3,
	LevelFour = 4,
	LevelFive = 5,
	EUGCArmorLevel_MAX = 6
};

// Object: Enum UGCExtend.EUGCArmorStatus
enum class EUGCArmorStatus : uint8_t
{
	Unequiped = 0,
	Equiped = 1,
	EUGCArmorStatus_MAX = 2
};

// Object: Enum UGCExtend.EUGCGameEquipmentSlot
enum class EUGCGameEquipmentSlot : uint8_t
{
	Unknown = 0,
	Normal = 1,
	MainWeapon = 2,
	SubWeapon = 3,
	MeleeWeapon = 4,
	Grenade1 = 5,
	Grenade2 = 6,
	LightAmmo = 7,
	HeavyAmmo = 8,
	ShotgunAmmo = 9,
	SniperAmmo = 10,
	RpgAmmo = 11,
	Medicine = 12,
	HeadShield = 13,
	BodyShield = 14,
	HPOverload = 15,
	DfGameBox = 16,
	DfGameBigBox = 17,
	Vehicle = 18,
	Clothes = 19,
	Bracelet = 20,
	Shoes = 21,
	BackAccessory = 22,
	CustomType1 = 23,
	CustomType2 = 24,
	CustomType3 = 25,
	CustomType4 = 26,
	CustomType5 = 27,
	CustomType6 = 28,
	CustomType7 = 29,
	CustomType8 = 30,
	CustomType9 = 31,
	CustomType10 = 32,
	EUGCGameEquipmentSlot_MAX = 33
};

// Object: Enum UGCExtend.EUGCPickUpError
enum class EUGCPickUpError : uint16_t
{
	PickUpSuccess = 0,
	InvalidPlayer = 1001,
	InvalidProp = 1002,
	ROLE_NotAuthority = 1010,
	InventoryFull = 1020,
	PickUpFail = 1099,
	EUGCPickUpError_MAX = 1100
};

// Package: UGCScripting
// Enums: 1

// Object: Enum UGCScripting.EUGCScriptPropertyOption
enum class EUGCScriptPropertyOption : uint8_t
{
	None = 0,
	Add = 1,
	Change = 2,
	Remove = 3,
	EUGCScriptPropertyOption_MAX = 4
};

// Package: UGCFile
// Enums: 1

// Object: Enum UGCFile.EUGCFileType
enum class EUGCFileType : uint8_t
{
	ZIP = 0,
	DATABASE = 1,
	EUGCFileType_MAX = 2
};

// Package: GameplayTasks
// Enums: 3

// Object: Enum GameplayTasks.ETaskResourceOverlapPolicy
enum class ETaskResourceOverlapPolicy : uint8_t
{
	StartOnTop = 0,
	StartAtEnd = 1,
	ETaskResourceOverlapPolicy_MAX = 2
};

// Object: Enum GameplayTasks.EGameplayTaskRunResult
enum class EGameplayTaskRunResult : uint8_t
{
	Error = 0,
	Failed = 1,
	Success_Paused = 2,
	Success_Active = 3,
	Success_Finished = 4,
	EGameplayTaskRunResult_MAX = 5
};

// Object: Enum GameplayTasks.EGameplayTaskState
enum class EGameplayTaskState : uint8_t
{
	Uninitialized = 0,
	AwaitingActivation = 1,
	Paused = 2,
	Active = 3,
	Finished = 4,
	EGameplayTaskState_MAX = 5
};

// Package: GameplayAbilities
// Enums: 33

// Object: Enum GameplayAbilities.EGameplayEffectGrantedAbilityRemovePolicy
enum class EGameplayEffectGrantedAbilityRemovePolicy : uint8_t
{
	CancelAbilityImmediately = 0,
	RemoveAbilityOnEnd = 1,
	DoNothing = 2,
	EGameplayEffectGrantedAbilityRemovePolicy_MAX = 3
};

// Object: Enum GameplayAbilities.EGameplayEffectAttributeCaptureSource
enum class EGameplayEffectAttributeCaptureSource : uint8_t
{
	Source = 0,
	Target = 1,
	EGameplayEffectAttributeCaptureSource_MAX = 2
};

// Object: Enum GameplayAbilities.EGameplayAbilityActivationMode
enum class EGameplayAbilityActivationMode : uint8_t
{
	Authority = 0,
	NonAuthority = 1,
	Predicting = 2,
	Confirmed = 3,
	Rejected = 4,
	EGameplayAbilityActivationMode_MAX = 5
};

// Object: Enum GameplayAbilities.EAbilityGenericReplicatedEvent
enum class EAbilityGenericReplicatedEvent : uint8_t
{
	GenericConfirm = 0,
	GenericCancel = 1,
	InputPressed = 2,
	InputReleased = 3,
	GenericSignalFromClient = 4,
	GenericSignalFromServer = 5,
	GameCustom1 = 6,
	GameCustom2 = 7,
	GameCustom3 = 8,
	GameCustom4 = 9,
	GameCustom5 = 10,
	GameCustom6 = 11,
	MAX = 12
};

// Object: Enum GameplayAbilities.EGameplayEffectReplicationMode
enum class EGameplayEffectReplicationMode : uint8_t
{
	Minimal = 0,
	Mixed = 1,
	Full = 2,
	EGameplayEffectReplicationMode_MAX = 3
};

// Object: Enum GameplayAbilities.EAbilityTaskWaitState
enum class EAbilityTaskWaitState : uint8_t
{
	WaitingOnGame = 1,
	WaitingOnUser = 2,
	WaitingOnAvatar = 4,
	EAbilityTaskWaitState_MAX = 5
};

// Object: Enum GameplayAbilities.ERootMotionMoveToActorTargetOffsetType
enum class ERootMotionMoveToActorTargetOffsetType : uint8_t
{
	AlignFromTargetToSource = 0,
	AlignToTargetForward = 1,
	AlignToWorldSpace = 2,
	ERootMotionMoveToActorTargetOffsetType_MAX = 3
};

// Object: Enum GameplayAbilities.EAbilityTaskNetSyncType
enum class EAbilityTaskNetSyncType : uint8_t
{
	BothWait = 0,
	OnlyServerWait = 1,
	OnlyClientWait = 2,
	EAbilityTaskNetSyncType_MAX = 3
};

// Object: Enum GameplayAbilities.EWaitAttributeChangeComparison
enum class EWaitAttributeChangeComparison : uint8_t
{
	None = 0,
	GreaterThan = 1,
	LessThan = 2,
	GreaterThanOrEqualTo = 3,
	LessThanOrEqualTo = 4,
	NotEqualTo = 5,
	ExactlyEqualTo = 6,
	MAX = 7
};

// Object: Enum GameplayAbilities.EGameplayAbilityInputBinds
enum class EGameplayAbilityInputBinds : uint8_t
{
	Ability1 = 0,
	Ability2 = 1,
	Ability3 = 2,
	Ability4 = 3,
	Ability5 = 4,
	Ability6 = 5,
	Ability7 = 6,
	Ability8 = 7,
	Ability9 = 8,
	EGameplayAbilityInputBinds_MAX = 9
};

// Object: Enum GameplayAbilities.ETargetDataFilterSelf
enum class ETargetDataFilterSelf : uint8_t
{
	TDFS_Any = 0,
	TDFS_NoSelf = 1,
	TDFS_NoOthers = 2,
	TDFS_MAX = 3
};

// Object: Enum GameplayAbilities.EGameplayAbilityTargetingLocationType
enum class EGameplayAbilityTargetingLocationType : uint8_t
{
	LiteralTransform = 0,
	ActorTransform = 1,
	SocketTransform = 2,
	EGameplayAbilityTargetingLocationType_MAX = 3
};

// Object: Enum GameplayAbilities.EGameplayTargetingConfirmation
enum class EGameplayTargetingConfirmation : uint8_t
{
	Instant = 0,
	UserConfirmed = 1,
	Custom = 2,
	CustomMulti = 3,
	EGameplayTargetingConfirmation_MAX = 4
};

// Object: Enum GameplayAbilities.ERepAnimPositionMethod
enum class ERepAnimPositionMethod : uint8_t
{
	Position = 0,
	CurrentSectionId = 1,
	ERepAnimPositionMethod_MAX = 2
};

// Object: Enum GameplayAbilities.EGameplayAbilityTriggerSource
enum class EGameplayAbilityTriggerSource : uint8_t
{
	GameplayEvent = 0,
	OwnedTagAdded = 1,
	OwnedTagPresent = 2,
	EGameplayAbilityTriggerSource_MAX = 3
};

// Object: Enum GameplayAbilities.EGameplayAbilityReplicationPolicy
enum class EGameplayAbilityReplicationPolicy : uint8_t
{
	ReplicateNo = 0,
	ReplicateYes = 1,
	EGameplayAbilityReplicationPolicy_MAX = 2
};

// Object: Enum GameplayAbilities.EGameplayAbilityNetSecurityPolicy
enum class EGameplayAbilityNetSecurityPolicy : uint8_t
{
	ClientOrServer = 0,
	ServerOnlyExecution = 1,
	ServerOnlyTermination = 2,
	ServerOnly = 3,
	EGameplayAbilityNetSecurityPolicy_MAX = 4
};

// Object: Enum GameplayAbilities.EGameplayAbilityNetExecutionPolicy
enum class EGameplayAbilityNetExecutionPolicy : uint8_t
{
	LocalPredicted = 0,
	LocalOnly = 1,
	ServerInitiated = 2,
	ServerOnly = 3,
	EGameplayAbilityNetExecutionPolicy_MAX = 4
};

// Object: Enum GameplayAbilities.EGameplayAbilityInstancingPolicy
enum class EGameplayAbilityInstancingPolicy : uint8_t
{
	NonInstanced = 0,
	InstancedPerActor = 1,
	InstancedPerExecution = 2,
	EGameplayAbilityInstancingPolicy_MAX = 3
};

// Object: Enum GameplayAbilities.EGameplayCuePayloadType
enum class EGameplayCuePayloadType : uint8_t
{
	CueParameters = 0,
	FromSpec = 1,
	EGameplayCuePayloadType_MAX = 2
};

// Object: Enum GameplayAbilities.EGameplayEffectPeriodInhibitionRemovedPolicy
enum class EGameplayEffectPeriodInhibitionRemovedPolicy : uint8_t
{
	NeverReset = 0,
	ResetPeriod = 1,
	ExecuteAndResetPeriod = 2,
	EGameplayEffectPeriodInhibitionRemovedPolicy_MAX = 3
};

// Object: Enum GameplayAbilities.EGameplayEffectStackingExpirationPolicy
enum class EGameplayEffectStackingExpirationPolicy : uint8_t
{
	ClearEntireStack = 0,
	RemoveSingleStackAndRefreshDuration = 1,
	RefreshDuration = 2,
	EGameplayEffectStackingExpirationPolicy_MAX = 3
};

// Object: Enum GameplayAbilities.EGameplayEffectStackingPeriodPolicy
enum class EGameplayEffectStackingPeriodPolicy : uint8_t
{
	ResetOnSuccessfulApplication = 0,
	NeverReset = 1,
	EGameplayEffectStackingPeriodPolicy_MAX = 2
};

// Object: Enum GameplayAbilities.EGameplayEffectStackingDurationPolicy
enum class EGameplayEffectStackingDurationPolicy : uint8_t
{
	RefreshOnSuccessfulApplication = 0,
	NeverRefresh = 1,
	EGameplayEffectStackingDurationPolicy_MAX = 2
};

// Object: Enum GameplayAbilities.EGameplayEffectDurationType
enum class EGameplayEffectDurationType : uint8_t
{
	Instant = 0,
	Infinite = 1,
	HasDuration = 2,
	EGameplayEffectDurationType_MAX = 3
};

// Object: Enum GameplayAbilities.EGameplayEffectScopedModifierAggregatorType
enum class EGameplayEffectScopedModifierAggregatorType : uint8_t
{
	CapturedAttributeBacked = 0,
	Transient = 1,
	EGameplayEffectScopedModifierAggregatorType_MAX = 2
};

// Object: Enum GameplayAbilities.EAttributeBasedFloatCalculationType
enum class EAttributeBasedFloatCalculationType : uint8_t
{
	AttributeMagnitude = 0,
	AttributeBaseValue = 1,
	AttributeBonusMagnitude = 2,
	AttributeMagnitudeEvaluatedUpToChannel = 3,
	EAttributeBasedFloatCalculationType_MAX = 4
};

// Object: Enum GameplayAbilities.EGameplayEffectMagnitudeCalculation
enum class EGameplayEffectMagnitudeCalculation : uint8_t
{
	ScalableFloat = 0,
	AttributeBased = 1,
	CustomCalculationClass = 2,
	SetByCaller = 3,
	EGameplayEffectMagnitudeCalculation_MAX = 4
};

// Object: Enum GameplayAbilities.EGameplayTagEventType
enum class EGameplayTagEventType : uint8_t
{
	NewOrRemoved = 0,
	AnyCountChange = 1,
	EGameplayTagEventType_MAX = 2
};

// Object: Enum GameplayAbilities.EGameplayCueEvent
enum class EGameplayCueEvent : uint8_t
{
	OnActive = 0,
	WhileActive = 1,
	Executed = 2,
	Removed = 3,
	EGameplayCueEvent_MAX = 4
};

// Object: Enum GameplayAbilities.EGameplayEffectStackingType
enum class EGameplayEffectStackingType : uint8_t
{
	None = 0,
	AggregateBySource = 1,
	AggregateByTarget = 2,
	EGameplayEffectStackingType_MAX = 3
};

// Object: Enum GameplayAbilities.EGameplayModOp
enum class EGameplayModOp : uint8_t
{
	Additive = 0,
	Multiplicitive = 1,
	Division = 2,
	Override = 3,
	Max = 4
};

// Object: Enum GameplayAbilities.EGameplayModEvaluationChannel
enum class EGameplayModEvaluationChannel : uint8_t
{
	Channel0 = 0,
	Channel1 = 1,
	Channel2 = 2,
	Channel3 = 3,
	Channel4 = 4,
	Channel5 = 5,
	Channel6 = 6,
	Channel7 = 7,
	Channel8 = 8,
	Channel9 = 9,
	Channel_MAX = 10,
	EGameplayModEvaluationChannel_MAX = 11
};

// Package: BodyTypeAdaptation
// Enums: 1

// Object: Enum BodyTypeAdaptation.EMoeBodyType
enum class EMoeBodyType : uint8_t
{
	Doll = 0,
	TeenMale = 1,
	TeenFemale = 2,
	AdultMale = 3,
	AdultFemale = 4,
	Chiikawa = 5,
	PoseDriver = 6,
	EMoeBodyType_MAX = 7
};

// Package: AILabCommonPlugin
// Enums: 6

// Object: Enum AILabCommonPlugin.ELocalAIMonitorType
enum class ELocalAIMonitorType : uint8_t
{
	BDMT_LOCALDS_NONE = 0,
	BDMT_LOCALDS_RESULT = 1,
	BDMT_LOCALDS_ROBOT = 2,
	BDMT_LOCALDS_CONCURRENCY = 3,
	BDMT_LOCALDS_REQUESTTIME = 4,
	BDMT_LOCALDS_MAX = 5
};

// Object: Enum AILabCommonPlugin.ELocalAIConfigPathMode
enum class ELocalAIConfigPathMode : uint8_t
{
	InBinariesDir = 0,
	InContentDir = 1,
	InSavedDir = 2,
	InProjectDir = 3,
	AbsolutePath = 4,
	ELocalAIConfigPathMode_MAX = 5
};

// Object: Enum AILabCommonPlugin.ELocalAIDumpMode
enum class ELocalAIDumpMode : uint8_t
{
	DefaultMode = 0,
	DisableMode = 1,
	EnableMode = 2,
	ELocalAIDumpMode_MAX = 3
};

// Object: Enum AILabCommonPlugin.EAITrainMode
enum class EAITrainMode : uint8_t
{
	ServerTrain = 0,
	ClientTrain = 1,
	StandaloneTrain = 2,
	EAITrainMode_MAX = 3
};

// Object: Enum AILabCommonPlugin.EAISyncTransportMode
enum class EAISyncTransportMode : uint8_t
{
	ESyncFrameStateMode_Transparent = 0,
	ESyncFrameStateMode_Digest = 1,
	ESyncFrameStateMode_MAX = 2
};

// Object: Enum AILabCommonPlugin.EAIRouterMode
enum class EAIRouterMode : uint8_t
{
	ERouterMode_Debug = 0,
	ERouterMode_CL5 = 1,
	ERouterMode_MAX = 2
};

// Package: CinematicCamera
// Enums: 1

// Object: Enum CinematicCamera.ECameraFocusMethod
enum class ECameraFocusMethod : uint8_t
{
	DoNotOverride = 0,
	Manual = 1,
	Tracking = 2,
	Disable = 3,
	MAX = 4
};

// Package: MoeDataFlow
// Enums: 4

// Object: Enum MoeDataFlow.EDataFlowType
enum class EDataFlowType : uint8_t
{
	None = 0,
	MapActionFlow = 1,
	PlayerBattlePosFlow = 2,
	SuspectRoundDetailFlow = 3,
	DollRunRoundDetailFlow = 4,
	HideNSeekRoundDetailFlow = 5,
	LobbyActionFlow = 6,
	MapCommonActionFlow = 7,
	UGCMapActionFlow = 8,
	NewbieMapActionFlow = 9,
	FPSMapActionFlow = 10,
	BioRoundDetailFlow = 11,
	WeaponRoundDetailFlow = 12,
	DDPRoundDetailFlow = 13,
	UGCEditorTrialFlow = 14,
	FrontLineRoundDetailFlow = 15,
	BattleRoyaleRoundDetailFlow = 16,
	RacingRoundDetailFlow = 17,
	BioFightRoundDetailFlow = 18,
	MultiUGCMapActionFlow = 19,
	ChaseRoundDetailFlow = 20,
	BountyRoundDetailFlow = 21,
	DodgeBallRoundDetailFlow = 22,
	EvacuateRoundDetailFlow = 23,
	BulletScreenFlow = 24,
	ChaseSurrenderFlow = 25,
	RGReadyFlow = 26,
	RGRoundDetailFlow = 27,
	RGItemFlow = 28,
	RGTaskFlow = 29,
	RGMarkFlow = 30,
	RGMarkBoardFlow = 31,
	RGTalentActivateFlow = 32,
	ArenaHeroAttributesFlow = 33,
	ArenaEconomyFlow = 34,
	ArenaCardFlow = 35,
	ArenaAddPointFlow = 36,
	ArenaRoundResultFlow = 37,
	ArenaRoundDetailFlow = 38,
	UGCMallFlow = 39,
	UGCMoneyFlow = 40,
	UGCItemFlow = 41,
	ArenaBRRoundResultFlow = 42,
	ArenaBRRoundDetailFlow = 43,
	SuspectEmoticonUseFlow = 44,
	SuspectBattleBeginFlow = 45,
	UGCCodingFlow = 46,
	AFKFlow = 47,
	ArenaHangUpFlow = 48,
	ArenaBRCardFlow = 49,
	ArenaBattleBeginFlow = 50,
	TriggerExamineFlow = 51,
	TriggerGroupOpFlow = 52,
	SuspectEventFlow = 53,
	GameWatchingFlow = 54,
	SecRoundStartFlow = 55,
	SecRoundEndFlow = 56,
	SecItemGetFlow = 57,
	SecRoundDetailFlow = 58,
	SecAntiDataFlow = 59,
	SecVerifyFlow = 60,
	Canyon3v3RoundDetailFlow = 61,
	CanyonEatChickenRoundDetailFlow = 62,
	Canyon5v5RoundDetailFlow = 63,
	TycoonRoundDetailFlow = 64,
	SecRoundDetailStartFlow = 65,
	TycTDRoundDetailFlow = 66,
	TycTDSRoundDetailFlow = 67,
	GameFrameWork = 68,
	OMDRoundDetailFlow = 69,
	HOKSecRoundDetailFlow = 70,
	HOKFightMonsterFlow = 71,
	HOKFightSoldierFlow = 72,
	HOKFightTowerFlow = 73,
	HOKSkillFlow = 74,
	HOKBattleResultFlow = 75,
	HOKTowerDestroyFlow = 76,
	HOKSelectCardFlow = 77,
	ArenaTDRoundDetailFlow = 78,
	ArenaTDRoundResultFlow = 79,
	ArenaTDEconomyFlow = 80,
	ArenaTDTurretFlow = 81,
	HOKNewbieTaskFlow = 82,
	ArenaSurrenderFlow = 83,
	HOKSettingFlow = 84,
	MayDayRoundBeginFlow = 85,
	MayDayRoundDetailFlow = 86,
	MayDayRoundResultFlow = 87,
	MayDayMeetingFlow = 88,
	SPGameLoadingFlow = 89,
	SPPetCatchFlow = 90,
	SPPlayerPositionFlow = 91,
	SPDungeonUnlockFlow = 92,
	SPPlayerDungeonBeginFlow = 93,
	SPPlayerDungeonEndFlow = 94,
	SPDungeonEndFlow = 95,
	EDataFlowType_MAX = 96
};

// Object: Enum MoeDataFlow.ECurrentGameplayType
enum class ECurrentGameplayType : uint8_t
{
	None = 0,
	MainMode = 1,
	UGC = 2,
	DDP = 3,
	BioChase = 4,
	Gun = 5,
	HideAndSeek = 6,
	Werewolf = 7,
	DollsRun = 8,
	Lobby = 9,
	UGCEditor = 10,
	GunGameTDM = 11,
	Newbie = 12,
	GunGameBPM = 13,
	BattleRoyaleGame = 14,
	JSRunningGame = 15,
	Watching = 16,
	Tycoon = 17,
	GunGameKC = 18,
	MoeChase = 19,
	DDB = 20,
	Df = 21,
	DND = 22,
	OMD = 23,
	Roguelike = 24,
	Arena = 25,
	MobaTower = 27,
	HOK = 28,
	MayDay = 29,
	StarP = 30,
	ECurrentGameplayType_MAX = 31
};

// Object: Enum MoeDataFlow.EDataFlowReceiverType
enum class EDataFlowReceiverType : uint8_t
{
	UserResearch = 0,
	Sec = 1,
	EDataFlowReceiverType_MAX = 2
};

// Object: Enum MoeDataFlow.EMoeDataFlowReportMode
enum class EMoeDataFlowReportMode : uint8_t
{
	SendBattleTLog = 0,
	PushCollectSvrUDP = 1,
	EMoeDataFlowReportMode_MAX = 2
};

// Package: MoeUMGExtend
// Enums: 1

// Object: Enum MoeUMGExtend.ELGTextMode
enum class ELGTextMode : uint8_t
{
	None = 0,
	Wrap = 1,
	Truncation = 2,
	Shrink = 3,
	Omit = 4,
	Roll = 5,
	LGWrap = 6,
	Roll_For_AllVersion = 7,
	Collapsed = 8,
	ELGTextMode_MAX = 9
};

// Package: OnlineSubsystemUtils
// Enums: 7

// Object: Enum OnlineSubsystemUtils.EInAppPurchaseStatus
enum class EInAppPurchaseStatus : uint8_t
{
	Invalid = 0,
	Failed = 1,
	Deferred = 2,
	Canceled = 3,
	Purchased = 4,
	Restored = 5,
	EInAppPurchaseStatus_MAX = 6
};

// Object: Enum OnlineSubsystemUtils.EOnlineProxyStoreOfferDiscountType
enum class EOnlineProxyStoreOfferDiscountType : uint8_t
{
	NotOnSale = 0,
	Percentage = 1,
	DiscountAmount = 2,
	PayAmount = 3,
	EOnlineProxyStoreOfferDiscountType_MAX = 4
};

// Object: Enum OnlineSubsystemUtils.EBeaconConnectionState
enum class EBeaconConnectionState : uint8_t
{
	Invalid = 0,
	Closed = 1,
	Pending = 2,
	Open = 3,
	EBeaconConnectionState_MAX = 4
};

// Object: Enum OnlineSubsystemUtils.EClientRequestType
enum class EClientRequestType : uint8_t
{
	NonePending = 0,
	ExistingSessionReservation = 1,
	ReservationUpdate = 2,
	EmptyServerReservation = 3,
	Reconnect = 4,
	Abandon = 5,
	ReservationRemoveMembers = 6,
	EClientRequestType_MAX = 7
};

// Object: Enum OnlineSubsystemUtils.EPartyReservationResult
enum class EPartyReservationResult : uint8_t
{
	NoResult = 0,
	RequestPending = 1,
	GeneralError = 2,
	PartyLimitReached = 3,
	IncorrectPlayerCount = 4,
	RequestTimedOut = 5,
	ReservationDuplicate = 6,
	ReservationNotFound = 7,
	ReservationAccepted = 8,
	ReservationDenied = 9,
	ReservationDenied_CrossPlayRestriction = 10,
	ReservationDenied_Banned = 11,
	ReservationRequestCanceled = 12,
	ReservationInvalid = 13,
	BadSessionId = 14,
	ReservationDenied_ContainsExistingPlayers = 15,
	EPartyReservationResult_MAX = 16
};

// Object: Enum OnlineSubsystemUtils.ESpectatorClientRequestType
enum class ESpectatorClientRequestType : uint8_t
{
	NonePending = 0,
	ExistingSessionReservation = 1,
	ReservationUpdate = 2,
	EmptyServerReservation = 3,
	Reconnect = 4,
	Abandon = 5,
	ESpectatorClientRequestType_MAX = 6
};

// Object: Enum OnlineSubsystemUtils.ESpectatorReservationResult
enum class ESpectatorReservationResult : uint8_t
{
	NoResult = 0,
	RequestPending = 1,
	GeneralError = 2,
	SpectatorLimitReached = 3,
	IncorrectPlayerCount = 4,
	RequestTimedOut = 5,
	ReservationDuplicate = 6,
	ReservationNotFound = 7,
	ReservationAccepted = 8,
	ReservationDenied = 9,
	ReservationDenied_CrossPlayRestriction = 10,
	ReservationDenied_Banned = 11,
	ReservationRequestCanceled = 12,
	ReservationInvalid = 13,
	BadSessionId = 14,
	ReservationDenied_ContainsExistingPlayers = 15,
	ESpectatorReservationResult_MAX = 16
};

// Package: PhysXVehicles
// Enums: 2

// Object: Enum PhysXVehicles.EWheelSweepType
enum class EWheelSweepType : uint8_t
{
	SimpleAndComplex = 0,
	Simple = 1,
	Complex = 2,
	EWheelSweepType_MAX = 3
};

// Object: Enum PhysXVehicles.EVehicleDifferential4W
enum class EVehicleDifferential4W : uint8_t
{
	LimitedSlip_4W = 0,
	LimitedSlip_FrontDrive = 1,
	LimitedSlip_RearDrive = 2,
	Open_4W = 3,
	Open_FrontDrive = 4,
	Open_RearDrive = 5,
	EVehicleDifferential4W_MAX = 6
};

// Package: SignificanceManager
// Enums: 1

// Object: Enum SignificanceManager.ESignificancePostFuncType
enum class ESignificancePostFuncType : uint8_t
{
	None = 0,
	Concurrent = 1,
	Sequential = 2,
	ESignificancePostFuncType_MAX = 3
};

// Package: PlaceableActor
// Enums: 65

// Object: Enum PlaceableActor.EMoveForwardAndBackState
enum class EMoveForwardAndBackState : uint8_t
{
	None = 0,
	MoveOut = 1,
	MoveBack = 2,
	Pause = 3,
	End = 4,
	EMoveForwardAndBackState_MAX = 5
};

// Object: Enum PlaceableActor.EParticleControlAttachPoint
enum class EParticleControlAttachPoint : uint8_t
{
	BottomCenter = 0,
	Center = 1,
	TopCenter = 2,
	EParticleControlAttachPoint_MAX = 3
};

// Object: Enum PlaceableActor.ESelectType
enum class ESelectType : uint8_t
{
	All = 0,
	Rand = 1,
	CullRand = 2,
	Order = 3,
	ReverseOrder = 4,
	Custom = 5,
	ESelectType_MAX = 6
};

// Object: Enum PlaceableActor.EMoeTraceType
enum class EMoeTraceType : uint8_t
{
	UseVelocity = 0,
	Constant = 1,
	Length = 2,
	EMoeTraceType_MAX = 3
};

// Object: Enum PlaceableActor.EMoeTraceShapeType
enum class EMoeTraceShapeType : uint8_t
{
	Line = 0,
	Sphere = 1,
	Box = 2,
	SelfPrimitiveComp = 3,
	EMoeTraceShapeType_MAX = 4
};

// Object: Enum PlaceableActor.EMoeAutoDoorState
enum class EMoeAutoDoorState : uint8_t
{
	IS_CLOSEED = 0,
	IS_OPENING = 1,
	IS_OPENED = 2,
	IS_CLOSING = 3,
	IS_MAX = 4
};

// Object: Enum PlaceableActor.EBaoLingMoveState
enum class EBaoLingMoveState : uint8_t
{
	Inner = 0,
	InnerToOuter = 1,
	Outer = 2,
	OuterToInner = 3,
	EBaoLingMoveState_MAX = 4
};

// Object: Enum PlaceableActor.EBasicRebornType
enum class EBasicRebornType : uint8_t
{
	RB_Amount = 0,
	RB_Point = 1,
	RB_Never = 2,
	RB_MAX = 3
};

// Object: Enum PlaceableActor.EBasicSpawnRuleType
enum class EBasicSpawnRuleType : uint8_t
{
	SR_Random = 0,
	SR_Sequence = 1,
	SR_MAX = 2
};

// Object: Enum PlaceableActor.EBoomerangState
enum class EBoomerangState : uint8_t
{
	None = 0,
	FlyOut = 1,
	FlyBack = 2,
	Pause = 3,
	EBoomerangState_MAX = 4
};

// Object: Enum PlaceableActor.EBeanGroupState
enum class EBeanGroupState : uint8_t
{
	Active = 0,
	PendingInActive = 1,
	InActive = 2,
	EBeanGroupState_MAX = 3
};

// Object: Enum PlaceableActor.EMoeC4BombState
enum class EMoeC4BombState : uint8_t
{
	None = 0,
	NoPick = 1,
	OnHand = 2,
	Using = 3,
	AfterUse = 4,
	FakeDestroy = 5,
	EMoeC4BombState_MAX = 6
};

// Object: Enum PlaceableActor.EMoeSkillButtonShowType
enum class EMoeSkillButtonShowType : uint8_t
{
	EmptyHide = 0,
	EmptyShow = 1,
	EMoeSkillButtonShowType_MAX = 2
};

// Object: Enum PlaceableActor.EMoeVoiceType
enum class EMoeVoiceType : uint8_t
{
	DEFAULT = 0,
	Emotion = 1,
	Other = 2,
	Grab = 3,
	EMoeVoiceType_MAX = 4
};

// Object: Enum PlaceableActor.ECheckPointType
enum class ECheckPointType : uint8_t
{
	ECT_CheckRecent = 0,
	ECT_CheckMaxIndex = 1,
	ECT_Max = 255
};

// Object: Enum PlaceableActor.EMoeColorMatchType
enum class EMoeColorMatchType : uint8_t
{
	None = 0,
	Blue = 1,
	Red = 2,
	Green = 3,
	Yellow = 4,
	EMoeColorMatchType_MAX = 5
};

// Object: Enum PlaceableActor.EPlatState
enum class EPlatState : uint8_t
{
	DefaultShow = 0,
	NormalShow = 1,
	Hidden = 2,
	ShowCountDown = 3,
	Destroy = 4,
	WaitNextGame = 5,
	QuestionShow = 6,
	ShowTips = 7,
	HideTips = 8,
	EPlatState_MAX = 9
};

// Object: Enum PlaceableActor.EGachaBallOpCode
enum class EGachaBallOpCode : uint8_t
{
	Leave = 0,
	Enter = 1,
	EGachaBallOpCode_MAX = 2
};

// Object: Enum PlaceableActor.EGachaBallErrorCode
enum class EGachaBallErrorCode : uint8_t
{
	NoError = 0,
	AlreadyInGachaBall = 1,
	GachaBallIsFull = 2,
	PlayerNotDriving = 3,
	InValidPlayer = 4,
	EGachaBallErrorCode_MAX = 5
};

// Object: Enum PlaceableActor.EConveyorCharType
enum class EConveyorCharType : uint8_t
{
	None = 0,
	Speedup = 1,
	NoneSpeedup = 2,
	EConveyorCharType_MAX = 3
};

// Object: Enum PlaceableActor.EConveyorType
enum class EConveyorType : uint8_t
{
	Line = 0,
	Circle = 1,
	RightTriange = 2,
	Sector = 3,
	EConveyorType_MAX = 4
};

// Object: Enum PlaceableActor.CrackerState
enum class ECrackerState : uint8_t
{
	Init = 0,
	Idle = 1,
	Fire = 2,
	Warning = 3,
	Explode = 4,
	Max = 5
};

// Object: Enum PlaceableActor.EMoeFishingAttachType
enum class EMoeFishingAttachType : uint8_t
{
	None = 0,
	DetachPhysical = 1,
	DetachNoPhysical = 2,
	AttachActor = 3,
	EMoeFishingAttachType_MAX = 4
};

// Object: Enum PlaceableActor.EMoeFishingState
enum class EMoeFishingState : uint8_t
{
	None = 0,
	NoPick = 1,
	OnHand = 2,
	PreThrow = 3,
	Throw = 4,
	Hit = 5,
	Pull = 6,
	EMoeFishingState_MAX = 7
};

// Object: Enum PlaceableActor.EFootballTeam
enum class EFootballTeam : uint8_t
{
	Red = 0,
	Blue = 1,
	EFootballTeam_MAX = 2
};

// Object: Enum PlaceableActor.EFountainState
enum class EFountainState : uint8_t
{
	None = 0,
	StandBy = 1,
	Active = 2,
	EFountainState_MAX = 3
};

// Object: Enum PlaceableActor.EMoeFPSAddScoreAreaPointDealType
enum class EMoeFPSAddScoreAreaPointDealType : uint8_t
{
	Add = 0,
	Subtract = 1,
	EMoeFPSAddScoreAreaPointDealType_MAX = 2
};

// Object: Enum PlaceableActor.EMoeFPSAddScoreAreaPointRestrictType
enum class EMoeFPSAddScoreAreaPointRestrictType : uint8_t
{
	AllPlayer = 0,
	TeamPlayer = 1,
	EMoeFPSAddScoreAreaPointRestrictType_MAX = 2
};

// Object: Enum PlaceableActor.EFuncPlatState
enum class EFuncPlatState : uint8_t
{
	Pause = 0,
	Run = 1,
	EFuncPlatState_MAX = 2
};

// Object: Enum PlaceableActor.EMoeHangGlideState
enum class EMoeHangGlideState : uint8_t
{
	Start = 0,
	Loop = 1,
	Stop = 2,
	StopFinish = 3,
	EMoeHangGlideState_MAX = 4
};

// Object: Enum PlaceableActor.EMoeLogicType
enum class EMoeLogicType : uint8_t
{
	Logic_And = 0,
	Logic_Or = 1,
	Logic_Non = 2,
	Logic_Xor = 3,
	Logic_XNor = 4,
	Logic_MAX = 5
};

// Object: Enum PlaceableActor.ESpawnDirect
enum class ESpawnDirect : uint8_t
{
	Default = 0,
	Outside = 1,
	Inside = 2,
	ESpawnDirect_MAX = 3
};

// Object: Enum PlaceableActor.EGroupFilterRule
enum class EGroupFilterRule : uint8_t
{
	All = 0,
	Rand = 1,
	RandByClass = 2,
	RandGroup = 3,
	EGroupFilterRule_MAX = 4
};

// Object: Enum PlaceableActor.ELaserState_HorM
enum class ELaserState_HorM : uint8_t
{
	WaitStart = 0,
	StartMove = 1,
	FinishMove = 2,
	ELaserState_MAX = 3
};

// Object: Enum PlaceableActor.EMultiPropFuncType
enum class EMultiPropFuncType : uint8_t
{
	None = 0,
	Center = 1,
	Left = 2,
	Right = 3,
	Down = 4,
	EMultiPropFuncType_MAX = 5
};

// Object: Enum PlaceableActor.EMusicFloorType
enum class EMusicFloorType : uint8_t
{
	EMFT_Melody = 0,
	EMFT_Rhythm = 1,
	EMFT_Bass = 2,
	EMFT_Effect = 3,
	EMFT_MAX = 4
};

// Object: Enum PlaceableActor.EOMDDamageCollisionAreaTriggerType
enum class EOMDDamageCollisionAreaTriggerType : uint8_t
{
	Unknown = 0,
	EDCATT_Once = 1,
	EDCATT_Multi = 2,
	EOMDDamageCollisionAreaTriggerType_MAX = 3
};

// Object: Enum PlaceableActor.EOMDDamageTargetType
enum class EOMDDamageTargetType : uint8_t
{
	Orcs = 0,
	Players = 1,
	AllActors = 2,
	EOMDDamageTargetType_MAX = 3
};

// Object: Enum PlaceableActor.EBeanState
enum class EBeanState : uint8_t
{
	None = 0,
	Active = 1,
	PendingInActive = 2,
	InActive = 3,
	EBeanState_MAX = 4
};

// Object: Enum PlaceableActor.ESpawnPointPickRule
enum class ESpawnPointPickRule : uint8_t
{
	None = 0,
	Probability = 1,
	Priority = 2,
	ESpawnPointPickRule_MAX = 3
};

// Object: Enum PlaceableActor.EParticleControlInheritOwnerProperty
enum class EParticleControlInheritOwnerProperty : uint32_t
{
	PC_Scale = 1,
	PC_Rotation = 2,
	PC_Translation = 4,
	PC_All = 65535,
	PC_MAX = 65536
};

// Object: Enum PlaceableActor.EPhysicsCompControlType
enum class EPhysicsCompControlType : uint8_t
{
	ControlType_None = 0,
	Physics_Enable = 1,
	Collision_Enable = 2,
	Visibility_Enable = 3,
	UnWalk_Enable = 4,
	Grab_Enable = 5,
	ControlType_MAX = 6,
	EPhysicsCompControlType_MAX = 7
};

// Object: Enum PlaceableActor.EPlasmaBallFixedState
enum class EPlasmaBallFixedState : uint8_t
{
	Close = 0,
	Alarm = 1,
	Open = 2,
	EPlasmaBallFixedState_MAX = 3
};

// Object: Enum PlaceableActor.ERandomRule
enum class ERandomRule : uint8_t
{
	Probability = 0,
	Order = 1,
	ERandomRule_MAX = 2
};

// Object: Enum PlaceableActor.EPortalState
enum class EPortalState : uint8_t
{
	Init = 0,
	Activate = 1,
	CanTrigger = 2,
	Trigger = 3,
	Finish = 4,
	EPortalState_MAX = 5
};

// Object: Enum PlaceableActor.ESpawnMethodType
enum class ESpawnMethodType : uint8_t
{
	NoSpecial = 0,
	SpecialInRoads = 1,
	ESpawnMethodType_MAX = 2
};

// Object: Enum PlaceableActor.EReelMoveAxis
enum class EReelMoveAxis : uint8_t
{
	X = 0,
	Y = 1,
	Z = 2,
	ReversedX = 3,
	ReversedY = 4,
	ReversedZ = 5,
	EReelMoveAxis_MAX = 6
};

// Object: Enum PlaceableActor.ERotatePlatType
enum class ERotatePlatType : uint8_t
{
	Auto = 1,
	Trigger = 2,
	TriggerAndReset = 3,
	ERotatePlatType_MAX = 4
};

// Object: Enum PlaceableActor.EMoeScoreAddType
enum class EMoeScoreAddType : uint8_t
{
	Trigger = 0,
	Grab = 1,
	TriggerArea = 2,
	EMoeScoreAddType_MAX = 3
};

// Object: Enum PlaceableActor.ERebornType
enum class ERebornType : uint8_t
{
	RB_Never = 0,
	RB_Now = 1,
	RB_Delay = 2,
	RB_MAX = 3
};

// Object: Enum PlaceableActor.ESpawnEventType
enum class ESpawnEventType : uint8_t
{
	SE_Active = 0,
	SE_Pause = 1,
	SE_DeActive = 2,
	SE_Destroy = 3,
	SE_Reborn = 4,
	SE_LastSpawnActors = 5,
	SE_MAX = 6
};

// Object: Enum PlaceableActor.ESpawnRandomType
enum class ESpawnRandomType : uint8_t
{
	SR_Time = 0,
	SR_Count = 1,
	SR_MAX = 2
};

// Object: Enum PlaceableActor.ESpawnCheckType
enum class ESpawnCheckType : uint8_t
{
	SC_SceneClass = 0,
	SC_OwnerClass = 1,
	SC_MAX = 2
};

// Object: Enum PlaceableActor.EPoleMotionState
enum class EPoleMotionState : uint8_t
{
	Before = 0,
	Grasp = 1,
	LetGo = 2,
	EPoleMotionState_MAX = 3
};

// Object: Enum PlaceableActor.ESpawnRangeLimitType
enum class ESpawnRangeLimitType : uint8_t
{
	SL_MinDistance = 0,
	SL_MaxSpawnNum = 1,
	SL_MAX = 2
};

// Object: Enum PlaceableActor.ESpawnPointType
enum class ESpawnPointType : uint8_t
{
	SP_Point = 0,
	SP_Box = 1,
	SP_MAX = 2
};

// Object: Enum PlaceableActor.EMoeSubstituteState
enum class EMoeSubstituteState : uint8_t
{
	None = 0,
	Transfer = 2,
	End = 3,
	EMoeSubstituteState_MAX = 4
};

// Object: Enum PlaceableActor.ETimingPlatState
enum class ETimingPlatState : uint8_t
{
	OnBeforeAppear = 0,
	OnAppeared = 1,
	OnBeforeDisappear = 2,
	OnDisappeared = 3,
	ETimingPlatState_MAX = 4
};

// Object: Enum PlaceableActor.ETimingPlatLoopState
enum class ETimingPlatLoopState : uint8_t
{
	OnBeforeAppear = 0,
	OnAppeared = 1,
	OnBeforeDisappear = 2,
	OnDisappeared = 3,
	ETimingPlatLoopState_MAX = 4
};

// Object: Enum PlaceableActor.EMoeUIPopType
enum class EMoeUIPopType : uint8_t
{
	Tips = 0,
	TipsWithCount = 1,
	OnlyCount = 2,
	EMoeUIPopType_MAX = 3
};

// Object: Enum PlaceableActor.EWarningType
enum class EWarningType : uint8_t
{
	None = 0,
	Decal = 1,
	ChangeColor = 2,
	MeshShake = 3,
	Max = 4
};

// Object: Enum PlaceableActor.EPlaceableActorRuleTriggerType
enum class EPlaceableActorRuleTriggerType : uint8_t
{
	ONBEGINPLAY = 0,
	ONACTIVATE = 1,
	ONDEACTIVATE = 2,
	GAMEPLAYTAG = 3,
	EPlaceableActorRuleTriggerType_MAX = 4
};

// Object: Enum PlaceableActor.EStretchInType
enum class EStretchInType : uint8_t
{
	ToStart = 0,
	ToEnd = 1,
	EStretchInType_MAX = 2
};

// Object: Enum PlaceableActor.EVehicleResCode
enum class EVehicleResCode : uint8_t
{
	Success = 0,
	AlreadyInVehicle = 1,
	Full = 2,
	NoVechicle = 3,
	DrivingIsNone = 4,
	PlayerIsNull = 5,
	NotInVehicle = 6,
	EVehicleResCode_MAX = 7
};

// Object: Enum PlaceableActor.EVehicleRPCState
enum class EVehicleRPCState : uint8_t
{
	OutSide = 0,
	InSide = 1,
	EVehicleRPCState_MAX = 2
};

// Package: MoeFPSBase
// Enums: 38

// Object: Enum MoeFPSBase.EMonsterAbilityState
enum class EMonsterAbilityState : uint8_t
{
	AbilityStart = 0,
	AbilityFire = 1,
	AbilityEnd = 2,
	NoneAbility = 3,
	EMonsterAbilityState_MAX = 4
};

// Object: Enum MoeFPSBase.EMonsterChaseType
enum class EMonsterChaseType : uint8_t
{
	Active = 0,
	Passive = 1,
	EMonsterChaseType_MAX = 2
};

// Object: Enum MoeFPSBase.EMonsterGrade
enum class EMonsterGrade : uint8_t
{
	Weak = 0,
	Elite = 1,
	Boss = 2,
	Chest = 3,
	EMonsterGrade_MAX = 4
};

// Object: Enum MoeFPSBase.EMiniMapSoundType
enum class EMiniMapSoundType : uint8_t
{
	Unknown = 0,
	Fire = 1,
	Explode = 2,
	Footstep = 3,
	Vehicle = 4,
	EMiniMapSoundType_MAX = 5
};

// Object: Enum MoeFPSBase.EItemOptionType
enum class EItemOptionType : uint8_t
{
	None = 0,
	Drop = 1,
	Pick = 2,
	EItemOptionType_MAX = 3
};

// Object: Enum MoeFPSBase.EBrPointAmmoRepState
enum class EBrPointAmmoRepState : uint8_t
{
	None = 0,
	Active = 1,
	Picked = 2,
	EBrPointAmmoRepState_MAX = 3
};

// Object: Enum MoeFPSBase.EBrItemState
enum class EBrItemState : uint8_t
{
	Unknown = 0,
	EDeactivated = 1,
	EPicked = 2,
	EActivated = 3,
	EActiveInteract = 4,
	EBrItemState_MAX = 5
};

// Object: Enum MoeFPSBase.EBrPoisonCircleState
enum class EBrPoisonCircleState : uint8_t
{
	Initializing = 0,
	BeforeShrinking = 1,
	Shrinking = 2,
	EBrPoisonCircleState_MAX = 3
};

// Object: Enum MoeFPSBase.EFPSPropGetType
enum class EFPSPropGetType : uint8_t
{
	None = 0,
	PickUp = 1,
	PrepareTimeBuyItem = 2,
	PrepareTimeBuyPack = 3,
	VendingMachineBuyItem = 4,
	VendingMachineBuyPack = 5,
	RespawnBuy = 6,
	EFPSPropGetType_MAX = 7
};

// Object: Enum MoeFPSBase.EFPSWeaponGetType
enum class EFPSWeaponGetType : uint8_t
{
	None = 0,
	PickUp = 1,
	PrepareTimeBuyItem = 2,
	PrepareTimeBuyPack = 3,
	VendingMachineBuyItem = 4,
	VendingMachineBuyPack = 5,
	RespawnBuy = 6,
	EFPSWeaponGetType_MAX = 7
};

// Object: Enum MoeFPSBase.EFPSBaseTLogSkillType
enum class EFPSBaseTLogSkillType : uint8_t
{
	None = 0,
	Jump = 1,
	FPSReammo = 2,
	FPSSaveTeammate = 3,
	FPSSprint = 4,
	EFPSBaseTLogSkillType_MAX = 5
};

// Object: Enum MoeFPSBase.EFpsGlideState
enum class EFpsGlideState : uint8_t
{
	Inactive = 0,
	Start = 1,
	Loop = 2,
	End = 3,
	EFpsGlideState_MAX = 4
};

// Object: Enum MoeFPSBase.EDamageCollisionAreaTriggerType
enum class EDamageCollisionAreaTriggerType : uint8_t
{
	Unknown = 0,
	EDCATT_Once = 1,
	EDCATT_Multi = 2,
	EDamageCollisionAreaTriggerType_MAX = 3
};

// Object: Enum MoeFPSBase.EDamageCollisionAreaWeaponType
enum class EDamageCollisionAreaWeaponType : uint8_t
{
	Unknown = 0,
	EDCAWT_Gun = 1,
	EDCAWT_Melee = 2,
	EDCAWT_Explore = 3,
	EDCAWT_UGCSkill = 4,
	EDamageCollisionAreaWeaponType_MAX = 5
};

// Object: Enum MoeFPSBase.EGunGameKCSoundType
enum class EGunGameKCSoundType : uint8_t
{
	E_Nian_Born = 0,
	E_RedPacket_MultiDrop = 1,
	E_MAX = 2
};

// Object: Enum MoeFPSBase.ERoguelikeSkillCDState
enum class ERoguelikeSkillCDState : uint8_t
{
	Default = 0,
	Ready = 1,
	Charging = 2,
	Using = 3,
	CoolDown = 4,
	Forbid = 5,
	ERoguelikeSkillCDState_MAX = 6
};

// Object: Enum MoeFPSBase.ERoguelikeSkillChargeType
enum class ERoguelikeSkillChargeType : uint8_t
{
	Default = 0,
	Time = 1,
	Kill = 2,
	Hit = 3,
	TakeHit = 4,
	ERoguelikeSkillChargeType_MAX = 5
};

// Object: Enum MoeFPSBase.EHitReponseAreaMatchingWeaponType
enum class EHitReponseAreaMatchingWeaponType : uint8_t
{
	Unknown = 0,
	EHRA_Gun = 1,
	EHRA_Melee = 2,
	EHRA_EXplore = 4,
	EHRA_UGCSkill = 8,
	EHitReponseAreaMatchingWeaponType_MAX = 9
};

// Object: Enum MoeFPSBase.EMoeTankRotateState
enum class EMoeTankRotateState : uint8_t
{
	Moe_RotateIdle = 0,
	Moe_RotateLeft = 1,
	Moe_RotateRight = 2,
	Moe_MAX = 3
};

// Object: Enum MoeFPSBase.EDisperseModifierType
enum class EDisperseModifierType : uint8_t
{
	Add = 0,
	Subtract = 1,
	Multiply = 2,
	Divide = 3,
	EDisperseModifierType_MAX = 4
};

// Object: Enum MoeFPSBase.EDisperseRecoveryType
enum class EDisperseRecoveryType : uint8_t
{
	Add = 0,
	Max = 1
};

// Object: Enum MoeFPSBase.EPassiveAbilityTriggerLimit
enum class EPassiveAbilityTriggerLimit : uint8_t
{
	OnlyTriggerOnce = 0,
	NoLimit = 1,
	EPassiveAbilityTriggerLimit_MAX = 2
};

// Object: Enum MoeFPSBase.EPassiveAbilityTriggerType
enum class EPassiveAbilityTriggerType : uint8_t
{
	EqualTo = 0,
	NotEqualTo = 1,
	LessThanOrEqualTo = 2,
	GreaterThanOrEqualTo = 3,
	LessThan = 4,
	GreaterThan = 5,
	EPassiveAbilityTriggerType_MAX = 6
};

// Object: Enum MoeFPSBase.ERoguelikeTaskType
enum class ERoguelikeTaskType : uint8_t
{
	Unknown = 0,
	EWeekTask = 1,
	ESeasonDailyTask = 2,
	ESeasonWeekTask = 3,
	ESeasonTask = 4,
	ESeasonMileston = 5,
	ERoguelikeTaskType_MAX = 6
};

// Object: Enum MoeFPSBase.EMarkSpecialEffectTriggerCondition
enum class EMarkSpecialEffectTriggerCondition : uint8_t
{
	TC_InBoardLeft = 1,
	TC_InBoardRight = 2,
	TC_InBoardTop = 3,
	TC_InBoardBottom = 4,
	TC_NotInBoardLeft = 5,
	TC_NotInBoardRight = 6,
	TC_NotInBoardTop = 7,
	TC_NotInBoardBottom = 8,
	TC_MAX = 9
};

// Object: Enum MoeFPSBase.EMarkEntryTriggerType
enum class EMarkEntryTriggerType : uint8_t
{
	LowLevel = 3,
	MidLevel = 6,
	HighLevel = 9,
	EMarkEntryTriggerType_MAX = 10
};

// Object: Enum MoeFPSBase.ERoguelikeMarkEntryType
enum class ERoguelikeMarkEntryType : uint8_t
{
	ME_Health = 0,
	ME_Shield = 1,
	ME_Harvest = 2,
	ME_MAX = 3
};

// Object: Enum MoeFPSBase.ERoguelikeMarkBaseEffectType
enum class ERoguelikeMarkBaseEffectType : uint8_t
{
	MBE_Health = 0,
	MBE_Shield = 1,
	MBE_ExtraWeaponDamage = 2,
	MBE_MAX = 3
};

// Object: Enum MoeFPSBase.ERoguelikeMarkRarity
enum class ERoguelikeMarkRarity : uint8_t
{
	Level1 = 1,
	Level2 = 2,
	Level3 = 3,
	Level4 = 4,
	Level5 = 5,
	Level6 = 6,
	ERoguelikeMarkRarity_MAX = 7
};

// Object: Enum MoeFPSBase.ERoguelikeMarkBoardOp
enum class ERoguelikeMarkBoardOp : uint8_t
{
	Create = 0,
	Assembly = 1,
	Remove = 2,
	Delete = 3,
	ERoguelikeMarkBoardOp_MAX = 4
};

// Object: Enum MoeFPSBase.ERoguelikeMarkOp
enum class ERoguelikeMarkOp : uint8_t
{
	Obtain = 0,
	Upgrade = 1,
	Decompose = 2,
	Reshape = 3,
	Lock = 4,
	BreakThrough = 5,
	ERoguelikeMarkOp_MAX = 6
};

// Object: Enum MoeFPSBase.EMonsterStateType
enum class EMonsterStateType : uint8_t
{
	Normal = 0,
	Freeze = 1,
	Falling = 2,
	Dead = 3,
	EMonsterStateType_MAX = 4
};

// Object: Enum MoeFPSBase.EShootBuffSceneEffectType
enum class EShootBuffSceneEffectType : uint8_t
{
	Unknown = 0,
	Overload = 1,
	Burning = 2,
	Electric = 3,
	Cold = 4,
	Freeze = 5,
	IceRing = 6,
	FireRing = 7,
	IceFireRing = 8,
	Poison = 9,
	CounterAttack = 10,
	AddMonstereHP = 11,
	Accelerate = 12,
	MonsterElecFlag = 13,
	AddPlayerHP = 14,
	EShootBuffSceneEffectType_MAX = 15
};

// Object: Enum MoeFPSBase.EShootBuffEffectType
enum class EShootBuffEffectType : uint8_t
{
	ModifyAttribute = 0,
	VirtualAttribute = 1,
	EShootBuffEffectType_MAX = 2
};

// Object: Enum MoeFPSBase.EShootBuffTriggerType
enum class EShootBuffTriggerType : uint8_t
{
	ESBT_Immediately = 0,
	ESBT_Duration = 1,
	ESBT_Interval = 2,
	ESBT_Persistent = 3,
	ESBT_BuffStack = 4,
	ESBT_AttributeStack = 5,
	ESBT_Timer = 6,
	ESBT_MAX = 7
};

// Object: Enum MoeFPSBase.EShootBuffType
enum class EShootBuffType : uint8_t
{
	ESB_Unknown = 0,
	ESB_Health = 1,
	ESB_MaxHealth = 2,
	ESB_ShieldValue = 3,
	ESB_MaxShieldValue = 4,
	ESB_CriticalRate = 5,
	ESB_CriticalDamage = 6,
	ESB_BurningDamage = 7,
	ESB_GetCurrency = 8,
	ESB_AddAmmo = 9,
	ESB_AddMagazineAmmo = 10,
	ESB_GetExp = 11,
	ESB_UpgradeUp = 12,
	ESB_DirectDamage = 13,
	ESB_DirectMoveSpeed = 14,
	ESB_AddBuff = 15,
	ESB_AddAttack = 16,
	ESB_AddAttackRate = 17,
	ESB_AddAttackSpeed = 18,
	ESB_Overload = 19,
	ESB_DoOverloadDamage = 20,
	ESB_AddBuffMaxStack = 21,
	ESB_BurnDieExplosion = 23,
	ESB_BurnDamageRate = 24,
	ESB_PoisonExLiveTime = 25,
	ESB_AddGetDamageRate = 26,
	ESB_AddMagazineAmmoRate = 27,
	ESB_ReloadSpeed = 28,
	ESB_ReloadSpeedEmpty = 29,
	ESB_AddAmmoRate = 30,
	ESB_CriticalKillExplore = 31,
	ESB_PoisonCircle = 32,
	ESB_HeadDamageReduction = 33,
	ESB_BodyDamageReduction = 34,
	ESB_BeGrabDamageReduction = 35,
	ESB_BrOverCharge = 36,
	ESB_DyingReduceHealth = 37,
	ESB_LockedAttackRate = 38,
	ESB_LockedCriticalDamageRate = 39,
	ESB_ShowAttributeIntValue = 40,
	ESB_ShowAttributeFloatValue = 41,
	ESB_AddGetBombDamageRate = 42,
	ESB_FinalAddDamage = 43,
	ESB_DoDamageFire = 44,
	ESB_AddFire = 45,
	ESB_AddCold = 46,
	ESB_Cold = 47,
	ESB_Freeze = 48,
	ESB_ColdIntervalRate = 49,
	ESB_ColdRangeRate = 50,
	ESB_FreezeAddDamage = 51,
	ESB_ReduceFreezeDuration = 52,
	ESB_DoDamageElectric = 53,
	ESB_ElectricDamageRate = 54,
	ESB_ElectricInterval = 55,
	ESB_ElectricRange = 56,
	ESB_AddElectric = 57,
	ESB_ElectricTargetCount = 58,
	ESB_ElectricStoneBuff = 59,
	ESB_AddBeHealedRate = 60,
	ESB_AddHealRate = 61,
	ESB_AddBeHealedRateByHealth = 62,
	ESB_HealedByDamage = 63,
	ESB_HealedByKill = 64,
	ESB_AddHealedByDamageProb = 65,
	ESB_AddHealByKillValue = 66,
	ESB_AddBloodShieldValue = 67,
	ESB_AddMaxBloodShieldValue = 68,
	ESB_VirtualAddHealByDamageRate = 69,
	ESB_VirtualAddHealByDamageProb = 70,
	ESB_AddLandmineExInterval = 71,
	ESB_AddLandmineExDamage = 72,
	ESB_AddLandmineExBombDiameter = 73,
	ESB_AddLandmineAddBuffId = 74,
	ESB_IntervalPlaceLandmine = 75,
	ESB_AddBombDamageRate = 76,
	ESB_AddBombDiameter = 77,
	ESB_ExBombDamageRate = 78,
	ESB_ExBombDiameterRate = 79,
	ESB_SubProjectileDamageRate = 80,
	ESB_SetRechargeMaxCount = 81,
	ESB_AddRechargeByTime = 82,
	ESB_AddRechargeCount = 83,
	ESB_AddRechargeSpeed = 84,
	ESB_AddStandRechargeSpeed = 85,
	ESB_SetIsRecharge = 86,
	ESB_LevelStartGetGoldRate = 87,
	ESB_AddGetGoldRate = 88,
	ESB_ShopDiscountRate = 89,
	ESB_VirtualAddAttackSpeed = 90,
	ESB_CalculateStandTime = 91,
	ESB_AddReduceDamageRate = 92,
	ESB_AddStandAttr = 93,
	ESB_CounterAttackRate = 94,
	ESB_CounterAttackDoubleProb = 95,
	ESB_AddCounterAttackDamageRate = 96,
	ESB_DoAOECounterAttack = 97,
	ESB_AddTempShieldValue = 98,
	ESB_VirtualAddTempShield = 99,
	ESB_DoDamagePoison = 100,
	ESB_PoisonDamageRate = 101,
	ESB_PoisonSplit = 102,
	ESB_PoisonDirectKill = 103,
	ESB_AddPoisonExtraStack = 104,
	ESB_SetPoisonParameter = 105,
	ESB_PoisonBaseWeaponDamage = 106,
	ESB_FarAddDamageRate = 110,
	ESB_FarAddDamageDis = 111,
	ESB_NearAddDamageRate = 112,
	ESB_NearAddDamageNearDis = 113,
	ESB_NearAddDamageFarDis = 114,
	ESB_Invincible = 115,
	ESB_SkillRecharge = 116,
	ESB_ExShieldMaxRate = 120,
	ESB_ShieldMaxDamage = 121,
	ESB_ShieldReduceDamage = 122,
	ESB_ExMaxHpRate = 125,
	ESB_MoveSpeedAddDamage = 126,
	ESB_CalculateSafeTime = 127,
	ESB_SetRevival = 128,
	ESB_SkillShootRecharge = 129,
	ESB_UnlimitedFire = 130,
	ESB_ExSkillRechargeSpeed = 131,
	ESB_SkillCDReduceRate = 132,
	ESB_AddGetExRate = 133,
	ESB_StandAddShield = 134,
	ESB_AddBuffWhenDie = 135,
	ESB_MonsterBomb = 136,
	ESB_AddHealthByRate = 137,
	ESB_StrongCold = 138,
	ESB_AddSecondWeaponAmmo = 149,
	ESB_AddAmmoByTime = 150,
	ESB_AddMarkAttackRate = 151,
	ESB_EnableAddAmmoByTime = 152,
	ESB_AddBuffEmptyClip = 153,
	ESB_ExShotCost = 154,
	ESB_AddHighCostSpecial = 155,
	ESB_ProhibitChangeClip = 156,
	ESB_RecycleAmmoRate = 157,
	ESB_AddFocusCount = 158,
	ESB_AddBuffEmptyGiveWeapon = 159,
	ESB_ImproveBurnEffect = 160,
	ESB_ImproveColdEffect = 161,
	ESB_ImproveElecEffect = 162,
	ESB_ImprovePoisonEffect = 163,
	ESB_BeginElementAccum = 164,
	ESB_ElementDamageRate = 165,
	ESB_ExtraElementDamageTimes = 166,
	ESB_ExtraElementExploreTime = 167,
	ESB_AddWeaponRange = 168,
	ESB_ReducePunchX = 169,
	ESB_ReducePunchY = 170,
	ESB_ReduceHipSpread = 171,
	ESB_ReduceAimSpread = 172,
	ESB_SetHealedByDamageLimit = 173,
	ESB_AddBombRadiusPercent = 174,
	ESB_LevelStartGetGoldLimit = 175,
	ESB_BurnExplore = 176,
	ESB_BurnLevelRate = 177,
	ESB_ElecLevelRate = 178,
	ESB_DoColdDamage = 179,
	ESB_AutoAddAmmoRate = 180,
	ESB_DoBuffDuration = 181,
	ESB_UGCSkillDizziness = 182,
	ESB_UGCSkillCharmed = 183,
	ESB_UGCSkillKnockup = 184,
	ESB_UGCSkillEndure = 185,
	ESB_MAX = 186
};

// Object: Enum MoeFPSBase.ERingBuffEffectType
enum class ERingBuffEffectType : uint8_t
{
	FireIceCombo = 1,
	IceOnly = 2,
	FireOnly = 3,
	None = 4,
	ERingBuffEffectType_MAX = 5
};

// Object: Enum MoeFPSBase.ETacticalRadarOutType
enum class ETacticalRadarOutType : uint8_t
{
	SmallerY = 0,
	GreaterX = 1,
	GreaterY = 2,
	SmallerX = 3,
	ETacticalRadarOutType_MAX = 4
};

// Package: LetsGoAvatarMerge
// Enums: 9

// Object: Enum LetsGoAvatarMerge.EMoeCharDecorateModelType
enum class EMoeCharDecorateModelType : uint8_t
{
	None = 0,
	StaticMesh = 1,
	SkeletalMesh = 2,
	EMoeCharDecorateModelType_MAX = 3
};

// Object: Enum LetsGoAvatarMerge.EMoeSuitStaticMeshType
enum class EMoeSuitStaticMeshType : uint16_t
{
	FaceWear = 0,
	HeadWear = 1,
	BackWear = 2,
	CrownWear = 3,
	FaceShield = 4,
	HandWear = 5,
	SuitWear = 6,
	InvalidType = 255,
	EMoeSuitStaticMeshType_MAX = 256
};

// Object: Enum LetsGoAvatarMerge.EMoeCharDecorateType
enum class EMoeCharDecorateType : uint16_t
{
	FaceWear = 0,
	HeadWear = 1,
	BackWear = 2,
	CrownWear = 3,
	FaceShield = 4,
	HandWear = 5,
	WaistWear = 6,
	FreeWear = 7,
	InvalidType = 255,
	EMoeCharDecorateType_MAX = 256
};

// Object: Enum LetsGoAvatarMerge.EMoeAvatarQuality
enum class EMoeAvatarQuality : uint8_t
{
	High = 0,
	Middle = 1,
	Low = 2,
	SP = 3,
	PLDIY = 4,
	SPHD = 5,
	EMoeAvatarQuality_MAX = 6
};

// Object: Enum LetsGoAvatarMerge.ELetsGoTexturePartType
enum class ELetsGoTexturePartType : uint8_t
{
	PT_Face = 0,
	PT_Skin = 1,
	PT_MAX = 2
};

// Object: Enum LetsGoAvatarMerge.ELetsGoMeshPartType
enum class ELetsGoMeshPartType : uint8_t
{
	PT_Face = 0,
	PT_Skin = 1,
	PT_Top = 2,
	PT_Bottom = 3,
	PT_Hand = 4,
	PT_MAX = 5
};

// Object: Enum LetsGoAvatarMerge.EMergeContextState
enum class EMergeContextState : uint8_t
{
	Created = 0,
	AsyncLoadingResource = 1,
	ResourceLoadFinished = 2,
	AssetCollected = 3,
	MergeResourceCreated = 4,
	ReadyToDispatch = 5,
	Dispatched = 6,
	Finished = 7,
	EMergeContextState_MAX = 8
};

// Object: Enum LetsGoAvatarMerge.ESPTextureMergeType
enum class ESPTextureMergeType : uint8_t
{
	SP_TextureMergeType_BaseColor = 0,
	SP_TextureMergeType_Normal = 1,
	SP_TextureMergeType_ARM = 2,
	SP_TextureMergeType_Mask = 3,
	SP_TextureMergeType_MPLUT = 4,
	SP_TextureMergeType_MAX = 5
};

// Object: Enum LetsGoAvatarMerge.EPLTextureMergeType
enum class EPLTextureMergeType : uint8_t
{
	PL_TextureMergeType_BaseColor = 0,
	PL_TextureMergeType_Normal = 1,
	PL_TextureMergeType_ARM = 2,
	PL_TextureMergeType_Mask = 3,
	PL_TextureMergeType_END = 4,
	PL_TextureMergeType_MAX = 5
};

// Package: LetsGoAvatarAISuit
// Enums: 2

// Object: Enum LetsGoAvatarAISuit.EDebugInfo
enum class EDebugInfo : uint8_t
{
	None = 0,
	KeepTask = 1,
	SaveDownloadImage = 2,
	SaveDownloadUnCompressedImage = 4,
	SaveGenerateJson = 8,
	EDebugInfo_MAX = 9
};

// Object: Enum LetsGoAvatarAISuit.EEquipQuality
enum class EEquipQuality : uint8_t
{
	UnknowEquip = 0,
	WhiteEquip = 1,
	GreenEquip = 2,
	BlueEquip = 3,
	PurpleEquip = 4,
	OrangeEquip = 5,
	EEquipQuality_MAX = 6
};

// Package: AkAudio
// Enums: 27

// Object: Enum AkAudio.EAkCallbackType
enum class EAkCallbackType : uint8_t
{
	EndOfEvent = 0,
	Marker = 2,
	Duration = 3,
	Starvation = 5,
	MusicPlayStarted = 7,
	MusicSyncBeat = 8,
	MusicSyncBar = 9,
	MusicSyncEntry = 10,
	MusicSyncExit = 11,
	MusicSyncGrid = 12,
	MusicSyncUserCue = 13,
	MusicSyncPoint = 14,
	MIDIEvent = 16,
	EAkCallbackType_MAX = 17
};

// Object: Enum AkAudio.EAkResult
enum class EAkResult : uint8_t
{
	NotImplemented = 0,
	Success = 1,
	Fail = 2,
	PartialSuccess = 3,
	NotCompatible = 4,
	AlreadyConnected = 5,
	InvalidFile = 7,
	AudioFileHeaderTooLarge = 8,
	MaxReached = 9,
	InvalidID = 14,
	IDNotFound = 15,
	InvalidInstanceID = 16,
	NoMoreData = 17,
	InvalidStateGroup = 20,
	ChildAlreadyHasAParent = 21,
	InvalidLanguage = 22,
	CannotAddItseflAsAChild = 23,
	InvalidParameter = 31,
	ElementAlreadyInList = 35,
	PathNotFound = 36,
	PathNoVertices = 37,
	PathNotRunning = 38,
	PathNotPaused = 39,
	PathNodeAlreadyInList = 40,
	PathNodeNotInList = 41,
	DataNeeded = 43,
	NoDataNeeded = 44,
	DataReady = 45,
	NoDataReady = 46,
	InsufficientMemory = 52,
	Cancelled = 53,
	UnknownBankID = 54,
	BankReadError = 56,
	InvalidSwitchType = 57,
	FormatNotReady = 63,
	WrongBankVersion = 64,
	FileNotFound = 66,
	DeviceNotReady = 67,
	BankAlreadyLoaded = 69,
	RenderedFX = 71,
	ProcessNeeded = 72,
	ProcessDone = 73,
	MemManagerNotInitialized = 74,
	StreamMgrNotInitialized = 75,
	SSEInstructionsNotSupported = 76,
	Busy = 77,
	UnsupportedChannelConfig = 78,
	PluginMediaNotAvailable = 79,
	MustBeVirtualized = 80,
	CommandTooLarge = 81,
	RejectedByFilter = 82,
	InvalidCustomPlatformName = 83,
	DLLCannotLoad = 84,
	DLLPathNotFound = 85,
	NoJavaVM = 86,
	OpenSLError = 87,
	PluginNotRegistered = 88,
	DataAlignmentError = 89,
	EAkResult_MAX = 90
};

// Object: Enum AkAudio.EAkAndroidAudioAPI
enum class EAkAndroidAudioAPI : uint8_t
{
	AAudio = 0,
	OpenSL_ES = 1,
	EAkAndroidAudioAPI_MAX = 2
};

// Object: Enum AkAudio.EAkAudioSessionMode
enum class EAkAudioSessionMode : uint8_t
{
	Default = 0,
	VoiceChat = 1,
	GameChat = 2,
	VideoRecording = 3,
	Measurement = 4,
	MoviePlayback = 5,
	VideoChat = 6,
	EAkAudioSessionMode_MAX = 7
};

// Object: Enum AkAudio.EAkAudioSessionCategoryOptions
enum class EAkAudioSessionCategoryOptions : uint8_t
{
	MixWithOthers = 0,
	DuckOthers = 1,
	AllowBluetooth = 2,
	DefaultToSpeaker = 3,
	EAkAudioSessionCategoryOptions_MAX = 4
};

// Object: Enum AkAudio.EAkAudioSessionCategory
enum class EAkAudioSessionCategory : uint8_t
{
	Ambient = 0,
	SoloAmbient = 1,
	PlayAndRecord = 2,
	EAkAudioSessionCategory_MAX = 3
};

// Object: Enum AkAudio.EReflectionFilterBits
enum class EReflectionFilterBits : uint8_t
{
	Wall = 0,
	Ceiling = 1,
	Floor = 2,
	EReflectionFilterBits_MAX = 3
};

// Object: Enum AkAudio.AkCodecId
enum class EAkCodecId : uint8_t
{
	None = 0,
	PCM = 1,
	ADPCM = 2,
	XMA = 3,
	Vorbis = 4,
	AAC = 10,
	ATRAC9 = 12,
	OpusNX = 17,
	AkOpus = 19,
	AkOpusWEM = 20,
	AkCodecId_MAX = 21
};

// Object: Enum AkAudio.EAkMidiCcValues
enum class EAkMidiCcValues : uint8_t
{
	AkMidiCcBankSelectCoarse = 0,
	AkMidiCcModWheelCoarse = 1,
	AkMidiCcBreathCtrlCoarse = 2,
	AkMidiCcCtrl3Coarse = 3,
	AkMidiCcFootPedalCoarse = 4,
	AkMidiCcPortamentoCoarse = 5,
	AkMidiCcDataEntryCoarse = 6,
	AkMidiCcVolumeCoarse = 7,
	AkMidiCcBalanceCoarse = 8,
	AkMidiCcCtrl9Coarse = 9,
	AkMidiCcPanPositionCoarse = 10,
	AkMidiCcExpressionCoarse = 11,
	AkMidiCcEffectCtrl1Coarse = 12,
	AkMidiCcEffectCtrl2Coarse = 13,
	AkMidiCcCtrl14Coarse = 14,
	AkMidiCcCtrl15Coarse = 15,
	AkMidiCcGenSlider1 = 16,
	AkMidiCcGenSlider2 = 17,
	AkMidiCcGenSlider3 = 18,
	AkMidiCcGenSlider4 = 19,
	AkMidiCcCtrl20Coarse = 20,
	AkMidiCcCtrl21Coarse = 21,
	AkMidiCcCtrl22Coarse = 22,
	AkMidiCcCtrl23Coarse = 23,
	AkMidiCcCtrl24Coarse = 24,
	AkMidiCcCtrl25Coarse = 25,
	AkMidiCcCtrl26Coarse = 26,
	AkMidiCcCtrl27Coarse = 27,
	AkMidiCcCtrl28Coarse = 28,
	AkMidiCcCtrl29Coarse = 29,
	AkMidiCcCtrl30Coarse = 30,
	AkMidiCcCtrl31Coarse = 31,
	AkMidiCcBankSelectFine = 32,
	AkMidiCcModWheelFine = 33,
	AkMidiCcBreathCtrlFine = 34,
	AkMidiCcCtrl3Fine = 35,
	AkMidiCcFootPedalFine = 36,
	AkMidiCcPortamentoFine = 37,
	AkMidiCcDataEntryFine = 38,
	AkMidiCcVolumeFine = 39,
	AkMidiCcBalanceFine = 40,
	AkMidiCcCtrl9Fine = 41,
	AkMidiCcPanPositionFine = 42,
	AkMidiCcExpressionFine = 43,
	AkMidiCcEffectCtrl1Fine = 44,
	AkMidiCcEffectCtrl2Fine = 45,
	AkMidiCcCtrl14Fine = 46,
	AkMidiCcCtrl15Fine = 47,
	AkMidiCcCtrl20Fine = 52,
	AkMidiCcCtrl21Fine = 53,
	AkMidiCcCtrl22Fine = 54,
	AkMidiCcCtrl23Fine = 55,
	AkMidiCcCtrl24Fine = 56,
	AkMidiCcCtrl25Fine = 57,
	AkMidiCcCtrl26Fine = 58,
	AkMidiCcCtrl27Fine = 59,
	AkMidiCcCtrl28Fine = 60,
	AkMidiCcCtrl29Fine = 61,
	AkMidiCcCtrl30Fine = 62,
	AkMidiCcCtrl31Fine = 63,
	AkMidiCcHoldPedal = 64,
	AkMidiCcPortamentoOnOff = 65,
	AkMidiCcSustenutoPedal = 66,
	AkMidiCcSoftPedal = 67,
	AkMidiCcLegatoPedal = 68,
	AkMidiCcHoldPedal2 = 69,
	AkMidiCcSoundVariation = 70,
	AkMidiCcSoundTimbre = 71,
	AkMidiCcSoundReleaseTime = 72,
	AkMidiCcSoundAttackTime = 73,
	AkMidiCcSoundBrightness = 74,
	AkMidiCcSoundCtrl6 = 75,
	AkMidiCcSoundCtrl7 = 76,
	AkMidiCcSoundCtrl8 = 77,
	AkMidiCcSoundCtrl9 = 78,
	AkMidiCcSoundCtrl10 = 79,
	AkMidiCcGeneralButton1 = 80,
	AkMidiCcGeneralButton2 = 81,
	AkMidiCcGeneralButton3 = 82,
	AkMidiCcGeneralButton4 = 83,
	AkMidiCcReverbLevel = 91,
	AkMidiCcTremoloLevel = 92,
	AkMidiCcChorusLevel = 93,
	AkMidiCcCelesteLevel = 94,
	AkMidiCcPhaserLevel = 95,
	AkMidiCcDataButtonP1 = 96,
	AkMidiCcDataButtonM1 = 97,
	AkMidiCcNonRegisterCoarse = 98,
	AkMidiCcNonRegisterFine = 99,
	AkMidiCcAllSoundOff = 120,
	AkMidiCcAllControllersOff = 121,
	AkMidiCcLocalKeyboard = 122,
	AkMidiCcAllNotesOff = 123,
	AkMidiCcOmniModeOff = 124,
	AkMidiCcOmniModeOn = 125,
	AkMidiCcOmniMonophonicOn = 126,
	AkMidiCcOmniPolyphonicOn = 127,
	EAkMidiCcValues_MAX = 128
};

// Object: Enum AkAudio.EAkMidiEventType
enum class EAkMidiEventType : uint16_t
{
	AkMidiEventTypeInvalid = 0,
	AkMidiEventTypeNoteOff = 128,
	AkMidiEventTypeNoteOn = 144,
	AkMidiEventTypeNoteAftertouch = 160,
	AkMidiEventTypeController = 176,
	AkMidiEventTypeProgramChange = 192,
	AkMidiEventTypeChannelAftertouch = 208,
	AkMidiEventTypePitchBend = 224,
	AkMidiEventTypeSysex = 240,
	AkMidiEventTypeEscape = 247,
	AkMidiEventTypeMeta = 255,
	EAkMidiEventType_MAX = 256
};

// Object: Enum AkAudio.ERTPCValueType
enum class ERTPCValueType : uint8_t
{
	Default = 0,
	Global = 1,
	GameObject = 2,
	PlayingID = 3,
	Unavailable = 4,
	ERTPCValueType_MAX = 5
};

// Object: Enum AkAudio.EAkCurveInterpolation
enum class EAkCurveInterpolation : uint8_t
{
	Log3 = 0,
	Sine = 1,
	Log1 = 2,
	InvSCurve = 3,
	Linear = 4,
	SCurve = 5,
	Exp1 = 6,
	SineRecip = 7,
	Exp3 = 8,
	LastFadeCurve = 9,
	Constant = 10,
	EAkCurveInterpolation_MAX = 11
};

// Object: Enum AkAudio.AkActionOnEventType
enum class EAkActionOnEventType : uint8_t
{
	Stop = 0,
	Pause = 1,
	Resume = 2,
	Break = 3,
	ReleaseEnvelope = 4,
	AkActionOnEventType_MAX = 5
};

// Object: Enum AkAudio.AkMultiPositionType
enum class EAkMultiPositionType : uint8_t
{
	SingleSource = 0,
	MultiSources = 1,
	MultiDirections = 2,
	AkMultiPositionType_MAX = 3
};

// Object: Enum AkAudio.AkSpeakerConfiguration
enum class EAkSpeakerConfiguration : uint32_t
{
	Ak_Speaker_Front_Left = 1,
	Ak_Speaker_Front_Right = 2,
	Ak_Speaker_Front_Center = 4,
	Ak_Speaker_Low_Frequency = 8,
	Ak_Speaker_Back_Left = 16,
	Ak_Speaker_Back_Right = 32,
	Ak_Speaker_Back_Center = 256,
	Ak_Speaker_Side_Left = 512,
	Ak_Speaker_Side_Right = 1024,
	Ak_Speaker_Top = 2048,
	Ak_Speaker_Height_Front_Left = 4096,
	Ak_Speaker_Height_Front_Center = 8192,
	Ak_Speaker_Height_Front_Right = 16384,
	Ak_Speaker_Height_Back_Left = 32768,
	Ak_Speaker_Height_Back_Center = 65536,
	Ak_Speaker_Height_Back_Right = 131072,
	Ak_Speaker_MAX = 131073
};

// Object: Enum AkAudio.AkChannelConfiguration
enum class EAkChannelConfiguration : uint8_t
{
	Ak_Parent = 0,
	Ak_LFE = 1,
	Ak_1 = 2,
	Ak_2 = 3,
	Ak_2 = 4,
	Ak_3 = 5,
	Ak_3 = 6,
	Ak_4 = 7,
	Ak_4 = 8,
	Ak_5 = 9,
	Ak_5 = 10,
	Ak_7 = 11,
	Ak_5_1 = 12,
	Ak_7_1 = 13,
	Ak_7_1 = 14,
	Ak_Auro_9 = 15,
	Ak_Auro_10 = 16,
	Ak_Auro_11 = 17,
	Ak_Auro_13 = 18,
	Ak_Ambisonics_1st_order = 19,
	Ak_Ambisonics_2nd_order = 20,
	Ak_Ambisonics_3rd_order = 21,
	Ak_MAX = 22
};

// Object: Enum AkAudio.AkAcousticPortalState
enum class EAkAcousticPortalState : uint8_t
{
	Closed = 0,
	Open = 1,
	AkAcousticPortalState_MAX = 2
};

// Object: Enum AkAudio.PanningRule
enum class EPanningRule : uint8_t
{
	PanningRule_Speakers = 0,
	PanningRule_Headphones = 1,
	PanningRule_MAX = 2
};

// Object: Enum AkAudio.AkMeshType
enum class EAkMeshType : uint8_t
{
	StaticMesh = 0,
	CollisionMesh = 1,
	AkMeshType_MAX = 2
};

// Object: Enum AkAudio.EAkHololensAudioAPI
enum class EAkHololensAudioAPI : uint8_t
{
	Wasapi = 0,
	XAudio2 = 1,
	DirectSound = 2,
	EAkHololensAudioAPI_MAX = 3
};

// Object: Enum AkAudio.EAkCommSystem
enum class EAkCommSystem : uint8_t
{
	Socket = 0,
	HTCS = 1,
	EAkCommSystem_MAX = 2
};

// Object: Enum AkAudio.EAkChannelMask
enum class EAkChannelMask : uint8_t
{
	FrontLeft = 0,
	FrontRight = 1,
	FrontCenter = 2,
	LowFrequency = 3,
	BackLeft = 4,
	BackRight = 5,
	BackCenter = 8,
	SideLeft = 9,
	SideRight = 10,
	Top = 11,
	HeightFrontLeft = 12,
	HeightFrontCenter = 13,
	HeightFrontRight = 14,
	HeightBackLeft = 15,
	HeightBackCenter = 16,
	HeightBackRight = 17,
	EAkChannelMask_MAX = 18
};

// Object: Enum AkAudio.EAkChannelConfigType
enum class EAkChannelConfigType : uint8_t
{
	Anonymous = 0,
	Standard = 1,
	Ambisonic = 2,
	EAkChannelConfigType_MAX = 3
};

// Object: Enum AkAudio.EAkDiffractionFlags
enum class EAkDiffractionFlags : uint8_t
{
	UseBuiltInParam = 0,
	UseObstruction = 1,
	CalcEmitterVirtualPosition = 3,
	EAkDiffractionFlags_MAX = 4
};

// Object: Enum AkAudio.EAkPanningRule
enum class EAkPanningRule : uint8_t
{
	Speakers = 0,
	Headphones = 1,
	EAkPanningRule_MAX = 2
};

// Object: Enum AkAudio.EAkOpenHarmonyAudioAPI
enum class EAkOpenHarmonyAudioAPI : uint8_t
{
	AAudio = 0,
	OpenSL_ES = 1,
	EAkOpenHarmonyAudioAPI_MAX = 2
};

// Object: Enum AkAudio.EAkWindowsAudioAPI
enum class EAkWindowsAudioAPI : uint8_t
{
	Wasapi = 0,
	XAudio2 = 1,
	DirectSound = 2,
	EAkWindowsAudioAPI_MAX = 3
};

// Package: MovieScene
// Enums: 14

// Object: Enum MovieScene.EMovieSceneKeyInterpolation
enum class EMovieSceneKeyInterpolation : uint8_t
{
	Auto = 0,
	User = 1,
	Break = 2,
	Linear = 3,
	Constant = 4,
	EMovieSceneKeyInterpolation_MAX = 5
};

// Object: Enum MovieScene.EMovieSceneBlendType
enum class EMovieSceneBlendType : uint8_t
{
	Invalid = 0,
	Absolute = 1,
	Additive = 2,
	Relative = 4,
	AdditiveFromBase = 8,
	EMovieSceneBlendType_MAX = 9
};

// Object: Enum MovieScene.EMovieSceneCompletionMode
enum class EMovieSceneCompletionMode : uint8_t
{
	KeepState = 0,
	RestoreState = 1,
	ProjectDefault = 2,
	EMovieSceneCompletionMode_MAX = 3
};

// Object: Enum MovieScene.EMovieSceneBuiltInEasing
enum class EMovieSceneBuiltInEasing : uint8_t
{
	Linear = 0,
	SinIn = 1,
	SinOut = 2,
	SinInOut = 3,
	QuadIn = 4,
	QuadOut = 5,
	QuadInOut = 6,
	CubicIn = 7,
	CubicOut = 8,
	CubicInOut = 9,
	QuartIn = 10,
	QuartOut = 11,
	QuartInOut = 12,
	QuintIn = 13,
	QuintOut = 14,
	QuintInOut = 15,
	ExpoIn = 16,
	ExpoOut = 17,
	ExpoInOut = 18,
	CircIn = 19,
	CircOut = 20,
	CircInOut = 21,
	EMovieSceneBuiltInEasing_MAX = 22
};

// Object: Enum MovieScene.EEvaluationMethod
enum class EEvaluationMethod : uint8_t
{
	Static = 0,
	Swept = 1,
	EEvaluationMethod_MAX = 2
};

// Object: Enum MovieScene.EMovieSceneSequenceFlags
enum class EMovieSceneSequenceFlags : uint8_t
{
	None = 0,
	Volatile = 1,
	BlockingEvaluation = 2,
	InheritedFlags = 3,
	EMovieSceneSequenceFlags_MAX = 4
};

// Object: Enum MovieScene.EUpdateClockSource
enum class EUpdateClockSource : uint8_t
{
	Tick = 0,
	Platform = 1,
	Audio = 2,
	RelativeTimecode = 3,
	Timecode = 4,
	Custom = 5,
	EUpdateClockSource_MAX = 6
};

// Object: Enum MovieScene.EMovieSceneEvaluationType
enum class EMovieSceneEvaluationType : uint8_t
{
	FrameLocked = 0,
	WithSubFrames = 1,
	EMovieSceneEvaluationType_MAX = 2
};

// Object: Enum MovieScene.EMovieScenePlayerStatus
enum class EMovieScenePlayerStatus : uint8_t
{
	Stopped = 0,
	Playing = 1,
	Recording = 2,
	Scrubbing = 3,
	Jumping = 4,
	Stepping = 5,
	Paused = 6,
	MAX = 7
};

// Object: Enum MovieScene.EMovieSceneObjectBindingSpace
enum class EMovieSceneObjectBindingSpace : uint8_t
{
	Local = 0,
	Root = 1,
	EMovieSceneObjectBindingSpace_MAX = 2
};

// Object: Enum MovieScene.ESectionEvaluationFlags
enum class ESectionEvaluationFlags : uint8_t
{
	None = 0,
	PreRoll = 1,
	PostRoll = 2,
	ESectionEvaluationFlags_MAX = 3
};

// Object: Enum MovieScene.EMovieScenePositionType
enum class EMovieScenePositionType : uint8_t
{
	Frame = 0,
	Time = 1,
	MarkedFrame = 2,
	EMovieScenePositionType_MAX = 3
};

// Object: Enum MovieScene.EUpdatePositionMethod
enum class EUpdatePositionMethod : uint8_t
{
	Play = 0,
	Jump = 1,
	Scrub = 2,
	EUpdatePositionMethod_MAX = 3
};

// Object: Enum MovieScene.ESpawnOwnership
enum class ESpawnOwnership : uint8_t
{
	InnerSequence = 0,
	MasterSequence = 1,
	External = 2,
	ESpawnOwnership_MAX = 3
};

// Package: FaceCustomization
// Enums: 3

// Object: Enum FaceCustomization.EAvatarGenderType
enum class EAvatarGenderType : uint8_t
{
	GENDER_UNINIT = 0,
	GENDER_MALE = 1,
	GENDER_FEMALE = 2,
	GENDER_TEST = 3,
	GENDER_MAX = 4
};

// Object: Enum FaceCustomization.EBodyType
enum class EBodyType : uint8_t
{
	EBodyNormal = 0,
	EBody40 = 1,
	EBody45 = 2,
	EBodyMax = 3,
	EBodyType_MAX = 4
};

// Object: Enum FaceCustomization.EModifyPortion
enum class EModifyPortion : uint8_t
{
	EyeNone = 0,
	EyeLid = 1,
	EyeLidUp = 2,
	EyeLidDown = 3,
	EyeLidUpOuterInner = 4,
	EyeLidDownOuterInner = 5,
	EyeLidOther = 6,
	Max = 7
};

// Package: MoeGameCommonRuntime
// Enums: 66

// Object: Enum MoeGameCommonRuntime.EJSCharacterState
enum class EJSCharacterState : uint8_t
{
	None = 0,
	Snowboarding = 1,
	Bike = 2,
	Giding = 3,
	Karting = 4,
	EJSCharacterState_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.EAdvanceBlendMode
enum class EAdvanceBlendMode : uint8_t
{
	Normal = 0,
	Color_Burn = 1,
	Color_Dodge = 2,
	Difference = 3,
	Exclusion = 4,
	Hardlight = 5,
	Liner_Burn = 6,
	Liner_Dodge = 7,
	Liner_Light = 8,
	Pin_Light = 9,
	Soft_Light = 10,
	Screen = 11,
	Lighten = 12,
	Darken = 13,
	Overlap = 14,
	Multiply = 15,
	EAdvanceBlendMode_MAX = 16
};

// Object: Enum MoeGameCommonRuntime.ETYCGuideArrowType
enum class ETYCGuideArrowType : uint8_t
{
	None = 0,
	Green = 1,
	Red = 2,
	WeakenedGreen = 3,
	WeakenedRed = 4,
	ETYCGuideArrowType_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.EDDPControlMultiplierSetType
enum class EDDPControlMultiplierSetType : uint8_t
{
	ALL = 0,
	Linear = 1,
	Angular = 2,
	EDDPControlMultiplierSetType_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EDDPAIUniqueToken
enum class EDDPAIUniqueToken : uint8_t
{
	Invalid = 0,
	WeaponToken = 1,
	DragCharacterToken = 2,
	EDDPAIUniqueToken_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EDDPAIConjuctType
enum class EDDPAIConjuctType : uint8_t
{
	And = 0,
	Or = 1,
	EDDPAIConjuctType_MAX = 2
};

// Object: Enum MoeGameCommonRuntime.EDDPAICheckOp
enum class EDDPAICheckOp : uint8_t
{
	Invalid = 0,
	EqualTo = 1,
	UnequalTo = 2,
	EDDPAICheckOp_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EDDPAICompareOp
enum class EDDPAICompareOp : uint8_t
{
	Invalid = 0,
	EqualTo = 1,
	UnequalTo = 2,
	GreaterThan = 3,
	LessThan = 4,
	GreaterThanOrEqualTo = 5,
	LessThanOrEqualTo = 6,
	EDDPAICompareOp_MAX = 7
};

// Object: Enum MoeGameCommonRuntime.EDDPAIPathMoveState
enum class EDDPAIPathMoveState : uint8_t
{
	Invalid = 0,
	Success = 1,
	Running = 2,
	Failure_PathFind = 3,
	Failure_Stuck = 4,
	EDDPAIPathMoveState_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.EPAAnimUpperLocoStatePriority
enum class EPAAnimUpperLocoStatePriority : uint8_t
{
	PA_None = 0,
	PA_ClavicleL = 1,
	PA_ClavicleR = 2,
	PA_ClavicleLR = 3,
	PA_UpperBody = 4,
	PA_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.EDDPGrabbedGPOType
enum class EDDPGrabbedGPOType : uint8_t
{
	DDPTTBat = 1,
	DDPElectrocuteGun = 2,
	DDPChomper = 3,
	DDPPineCone = 4,
	DDPCountDownBomb = 5,
	DDPPalmLeafFan = 6,
	DDPFreezeGun = 7,
	DDPWoodBox = 8,
	DDPMountaintopCrossBomb = 9,
	EDDPGrabbedGPOType_MAX = 10
};

// Object: Enum MoeGameCommonRuntime.EDDPCharDeathType
enum class EDDPCharDeathType : uint8_t
{
	DDPCharLightAttackedToDeath = 1,
	DDPCharHeaveyAttackedToDeath = 2,
	DDPCharPoisonedToDeath = 3,
	EDDPCharDeathType_MAX = 4
};

// Object: Enum MoeGameCommonRuntime.EDDPCharRoleStatusType
enum class EDDPCharRoleStatusType : uint8_t
{
	DDPCharRoleStatusGrabSearching = 1,
	DDPCharRoleStatusGrabPerforming = 2,
	DDPCharRoleStatusRagDoll = 3,
	DDPCharRoleStatusElectrocuted = 4,
	DDPCharRoleStatusNormallyGrabbed = 5,
	DDPCharRoleStatusLifted = 6,
	DDPCharRoleStatusPoisoned = 7,
	DDPCharRoleStatusHitByThunder = 8,
	DDPCharRoleStatusSuckedInByTornado = 9,
	DDPCharRoleStatusThrownOutByTornado = 10,
	DDPCharRoleStatusFrozen = 11,
	DDPCharRoleStatusBlownByPalmLeafFan = 12,
	DDPCharRoleStatusHitByCountDownBomb = 13,
	DDPCharRoleStatusHitByCountDownCrossBomb = 14,
	EDDPCharRoleStatusType_MAX = 15
};

// Object: Enum MoeGameCommonRuntime.EDDPCharAttackType
enum class EDDPCharAttackType : uint8_t
{
	DDPLightPunchAttack = 1,
	DDPFastFistAttack = 2,
	DDPJumpAttack = 3,
	DDPHeadAttack = 4,
	DDPTTBatAttack = 5,
	DDPElectrocuteGunFireAttack = 6,
	DDPChomperAttack = 7,
	DDPPineConeAttack = 8,
	DDPFreezeGunFireAttack = 9,
	DDPPalmLeafFanAttack = 10,
	EDDPCharAttackType_MAX = 11
};

// Object: Enum MoeGameCommonRuntime.EDDPCharSkillType
enum class EDDPCharSkillType : uint16_t
{
	DDPFist = 1000,
	DDPJumpAttack = 1001,
	DDPGrab = 1002,
	DDPGrabCharacter = 1003,
	DDPThrowCharacter = 1004,
	DDPMeleeWeaponAttack = 1005,
	DDPElectrocuteGunFire = 1006,
	DDPBeStunned = 1007,
	DDPElectrocuted = 1008,
	DDPAttackByHead = 1009,
	DDPEquipPan = 1010,
	DDPEquipElectrocuteGun = 1011,
	EDDPCharSkillType_MAX = 1012
};

// Object: Enum MoeGameCommonRuntime.EDDPGameMatchTypeBlueprintType
enum class EDDPGameMatchTypeBlueprintType : uint8_t
{
	Invalid = 0,
	DDP = 1,
	DDP_Airship = 2,
	DDP_MountainTop = 3,
	DDP_Tutorial = 11,
	DDP_Team = 12,
	DDP_Trial = 13,
	EDDPGameMatchTypeBlueprintType_MAX = 14
};

// Object: Enum MoeGameCommonRuntime.EDDPAITornadoRelativePosType
enum class EDDPAITornadoRelativePosType : uint8_t
{
	OutsideOfTornado = 0,
	BetweenInnerAndOuterTornado = 1,
	InsideInnerTornado = 2,
	EDDPAITornadoRelativePosType_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EDDPKeyLevel3
enum class EDDPKeyLevel3 : uint8_t
{
	ControlData = 0,
	ControlMultiplier = 1,
	ControlUseSkeletalAnimation = 2,
	ControlAngularDriveMode = 3,
	BodyPhysicsMovementType = 4,
	BodyGravityMultiplier = 5,
	BodyCollisionType = 6,
	BodyMassScale = 7,
	BodyInertiaTensorScale = 8,
	BodyPhysicalMaterial = 9,
	BodyPhysicsBlendWeight = 10,
	ConstraintProfile = 11,
	PullUpAngularStrengthMultiplier = 12,
	EDDPKeyLevel3_MAX = 13
};

// Object: Enum MoeGameCommonRuntime.EDDPKeyLevel2
enum class EDDPKeyLevel2 : uint8_t
{
	Null = 0,
	MainBody = 1,
	SubBody = 2,
	EDDPKeyLevel2_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EDDPKeyLevel1
enum class EDDPKeyLevel1 : uint8_t
{
	Null = 0,
	WorldSpace = 1,
	ParentSpace = 2,
	EDDPKeyLevel1_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EDDPKeyLevel0
enum class EDDPKeyLevel0 : uint8_t
{
	Control = 0,
	Body = 1,
	Constraint = 2,
	Global = 3,
	EDDPKeyLevel0_MAX = 4
};

// Object: Enum MoeGameCommonRuntime.EDDPProjectileCausedDeBuffType
enum class EDDPProjectileCausedDeBuffType : uint8_t
{
	Freeze = 1,
	Electrocute = 2,
	EDDPProjectileCausedDeBuffType_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EPAFistRuleDataType
enum class EPAFistRuleDataType : uint8_t
{
	E_InActive = 0,
	E_Ready = 1,
	E_Active = 2,
	E_Pending = 3,
	E_MAX = 4
};

// Object: Enum MoeGameCommonRuntime.EPAFistPhase
enum class EPAFistPhase : uint8_t
{
	E_None = 0,
	E_Froward = 1,
	E_Attack = 2,
	E_Back = 3,
	E_Immediately = 4,
	E_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.EPAFistHandType
enum class EPAFistHandType : uint8_t
{
	E_Right = 0,
	E_Left = 1,
	E_MAX = 2
};

// Object: Enum MoeGameCommonRuntime.EPAFistType
enum class EPAFistType : uint8_t
{
	E_Slow = 0,
	E_Fast = 1,
	E_Heavy = 2,
	E_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EDDPGameEliminatedReason
enum class EDDPGameEliminatedReason : uint8_t
{
	Dead = 0,
	ActiveQuit = 1,
	EDDPGameEliminatedReason_MAX = 2
};

// Object: Enum MoeGameCommonRuntime.EDDPAIGrabSetting
enum class EDDPAIGrabSetting : uint8_t
{
	AIGrabSetting_None = 0,
	AIGrabSetting_Equip = 1,
	AIGrabSetting_GrabCharacter = 2,
	AIGrabSetting_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EDDPGrabResult
enum class EDDPGrabResult : uint8_t
{
	Equip_Weapon = 0,
	Grab_GPO = 1,
	Grab_Character = 2,
	EDDPGrabResult_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EPALegRuleDataType
enum class EPALegRuleDataType : uint8_t
{
	E_InActive = 0,
	E_Ready = 1,
	E_Active = 2,
	E_Pending = 3,
	E_MAX = 4
};

// Object: Enum MoeGameCommonRuntime.EPALegPhase
enum class EPALegPhase : uint8_t
{
	E_None = 0,
	E_Froward = 1,
	E_Attack = 2,
	E_Back = 3,
	E_Immediately = 4,
	E_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.EPAFootType
enum class EPAFootType : uint8_t
{
	E_Right = 0,
	E_Left = 1,
	E_MAX = 2
};

// Object: Enum MoeGameCommonRuntime.EPALegAttackType
enum class EPALegAttackType : uint8_t
{
	E_Slow = 0,
	E_Fast = 1,
	E_MAX = 2
};

// Object: Enum MoeGameCommonRuntime.EDDPMountainTopBombState
enum class EDDPMountainTopBombState : uint8_t
{
	None = 0,
	Creating = 1,
	Falling = 2,
	Bombing = 3,
	EDDPMountainTopBombState_MAX = 4
};

// Object: Enum MoeGameCommonRuntime.EDDPMountainTopDirect
enum class EDDPMountainTopDirect : uint8_t
{
	Front = 0,
	Back = 1,
	Left = 2,
	Right = 3,
	Other = 4,
	EDDPMountainTopDirect_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.EPhysicsDebugData
enum class EPhysicsDebugData : uint8_t
{
	Bodies = 0,
	WorldControl = 1,
	ParentControl = 2,
	BodyModifiers = 3,
	PhysicsRecords = 4,
	EPhysicsDebugData_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.ESkillSlot
enum class ESkillSlot : uint8_t
{
	None = 0,
	Prop = 1,
	Grab = 2,
	Follow = 3,
	BagProp = 4,
	ESkillSlot_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.ECountDownDataType
enum class ECountDownDataType : uint8_t
{
	CountDown_Begin = 0,
	CountDown_End = 1,
	CountDown_MAX = 2
};

// Object: Enum MoeGameCommonRuntime.EDDpParticlePlayActorType
enum class EDDpParticlePlayActorType : uint8_t
{
	E_DDP_Character = 0,
	E_DDP_Camera = 1,
	E_DDP_MAX = 2
};

// Object: Enum MoeGameCommonRuntime.EDDpParticlePlayType
enum class EDDpParticlePlayType : uint8_t
{
	E_DDpParticlePlayType_Instant = 0,
	E_DDpParticlePlayType_Sustained = 1,
	E_DDpParticlePlayType_MAX = 2
};

// Object: Enum MoeGameCommonRuntime.EDDPUIState
enum class EDDPUIState : uint8_t
{
	E_UIState_Normal = 0,
	E_UIState_Activate = 1,
	E_UIState_Disable = 2,
	E_UIState_Success = 4,
	E_UIState_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.EDDPUI
enum class EDDPUI : uint8_t
{
	E_DDPUI_Dash = 0,
	E_DDPUI_Headbutt = 1,
	E_DDPUI_Jump = 2,
	E_DDPUI_Grab = 3,
	E_DDPUI_Run = 4,
	E_DDPUI_Dizzy = 5,
	E_DDPUI_Kick = 6,
	E_DDPUI_MAX = 7
};

// Object: Enum MoeGameCommonRuntime.EWindBehaviors
enum class EWindBehaviors : uint8_t
{
	Expand = 0,
	RandomMove = 1,
	Mixed = 2,
	EWindBehaviors_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EBindMode
enum class EBindMode : uint8_t
{
	Seat = 0,
	Bind = 1,
	Free = 2,
	EBindMode_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EHomeActorBpFlag
enum class EHomeActorBpFlag : uint8_t
{
	LogDetail = 0,
	DsExecuteReaction = 1,
	CharFixScale = 2,
	CheckBindCharLoc = 3,
	OnRepData = 4,
	OnActivated = 5,
	OnDeactivated = 6,
	IsLocalPlayer = 7,
	PlayerHandleTrigger = 8,
	PlayerHandleEffectArea = 9,
	OnReactionNtf = 10,
	OnServerBindChanged = 11,
	OnClientBindChanged = 12,
	IsPlayEnterAnim = 13,
	IsPlayExitAnim = 14,
	IsTriggerActive = 15,
	OnReqOperate = 16,
	CanEnter = 17,
	CanExit = 18,
	Server_DoEnter = 19,
	Server_DoExit = 20,
	Client_ResEnter = 21,
	Client_ResExit = 22,
	Client_ResOperate = 23,
	Client_CanOperate = 24,
	OnMulticastAction = 25,
	GetTipsImage = 26,
	IsShowOtherButton = 27,
	CanClickButton = 28,
	OnPlayerStateChange = 29,
	OnReactionForCol = 30,
	EHomeActorBpFlag_MAX = 31
};

// Object: Enum MoeGameCommonRuntime.EInteractCharacterCollisionType
enum class EInteractCharacterCollisionType : uint8_t
{
	ForCharacter = 0,
	NoCollision = 1,
	EInteractCharacterCollisionType_MAX = 2
};

// Object: Enum MoeGameCommonRuntime.EReactionDelegateMoment
enum class EReactionDelegateMoment : uint16_t
{
	InteractMontage_Start = 0,
	InteractMontage_End = 1,
	StandByMontage_Start = 2,
	StandByMontage_End = 3,
	ExitMontage_Start = 4,
	ExitMontage_End = 5,
	AttachToPoint_Complete = 6,
	DetachFromPoint_Complete = 7,
	UnKnown = 255,
	EReactionDelegateMoment_MAX = 256
};

// Object: Enum MoeGameCommonRuntime.EReactionProcess
enum class EReactionProcess : uint8_t
{
	None = 0,
	Enter = 1,
	Standby = 2,
	Exit = 4,
	EReactionProcess_MAX = 5
};

// Object: Enum MoeGameCommonRuntime.EJSTrackInitErrorType
enum class EJSTrackInitErrorType : uint8_t
{
	SpawnPointDest = 0,
	EndPointTooFarStartPoint = 1,
	StartLineTooFarStartPoint = 2,
	TrackMayBeDuplicateOrAuxiliary = 3,
	TooFarWithLastTrack = 4,
	OutsideStartLine = 5,
	InsideStartLine = 6,
	OutSideDestination = 7,
	EJSTrackInitErrorType_MAX = 8
};

// Object: Enum MoeGameCommonRuntime.EJSSpurtEffectType
enum class EJSSpurtEffectType : uint8_t
{
	None = 0,
	IdleSpurt = 1,
	RunSpurt = 2,
	AccSpurt = 3,
	DriftSpurt = 4,
	SecondDriftSpurt = 5,
	EJSSpurtEffectType_MAX = 6
};

// Object: Enum MoeGameCommonRuntime.EJSGroundType
enum class EJSGroundType : uint8_t
{
	None = 0,
	Snow = 1,
	Ice = 2,
	Rainbow = 3,
	Dust = 4,
	Flagging = 5,
	Plant = 6,
	Wood = 7,
	Sand = 8,
	Water = 9,
	Sea = 10,
	EJSGroundType_MAX = 11
};

// Object: Enum MoeGameCommonRuntime.EJSAttrModifyType
enum class EJSAttrModifyType : uint8_t
{
	Replace = 0,
	Add = 1,
	Addition = 2,
	EJSAttrModifyType_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EJSSpurtType
enum class EJSSpurtType : uint8_t
{
	None = 0,
	NormalSpurt = 1,
	SecondSpurt = 2,
	AirSpurt = 3,
	LandSpurt = 4,
	LittleDirftSpurt = 5,
	LittleDirftSecondSpurt = 6,
	StartGameSpurt = 7,
	BigSpurt = 8,
	HitSpurt = 9,
	EJSSpurtType_MAX = 10
};

// Object: Enum MoeGameCommonRuntime.EJSDriftState
enum class EJSDriftState : uint8_t
{
	None = 0,
	DriftStart = 1,
	DriftAbort = 2,
	Drifting = 3,
	DriftRestore = 4,
	DriftLasting = 5,
	DriftStop = 6,
	DriftSideway = 7,
	DriftTailFlick = 8,
	EJSDriftState_MAX = 9
};

// Object: Enum MoeGameCommonRuntime.ESnowboardingAnimState
enum class ESnowboardingAnimState : uint8_t
{
	Sliding = 0,
	Stand = 1,
	Acceration = 2,
	Acceration2 = 3,
	Acceration3 = 4,
	Jump = 5,
	BrakeLeft = 6,
	BrakeRight = 7,
	BackLeft = 8,
	BackBrakeLeft = 9,
	BackBrakeRight = 10,
	BackRight = 11,
	SlowSliding = 12,
	FastSliding = 13,
	JumpEnd = 14,
	TurnBrakeLeft = 15,
	AccSliding = 16,
	TurnBrakeRight = 17,
	SuperSpeedUp = 18,
	KartDrift = 19,
	KartTurn = 20,
	KartHitWall = 21,
	ESnowboardingAnimState_MAX = 22
};

// Object: Enum MoeGameCommonRuntime.EGlideAnimState
enum class EGlideAnimState : uint8_t
{
	Start = 0,
	Loop = 1,
	End = 2,
	EGlideAnimState_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EJSKartingEventType
enum class EJSKartingEventType : uint8_t
{
	None = 0,
	DoubleSpurt = 1,
	LittleDriftDoubleSpurt = 2,
	CW = 3,
	CWW = 4,
	WCW = 5,
	WCWW = 6,
	AirSpurt = 7,
	LandSpurt = 8,
	NitrogenLast = 9,
	FastAbort = 10,
	BestCWW = 11,
	BreakDrift = 12,
	BreakDoubleSpurt = 13,
	SidewaysDrift = 14,
	StopDrift = 15,
	BrakeBreakNitrogen = 16,
	TailFlickDrift = 17,
	AdvanceCWW = 18,
	HitSpurt = 19,
	CatapultTailFlick = 20,
	EJSKartingEventType_MAX = 21
};

// Object: Enum MoeGameCommonRuntime.EJSSpurtComboType
enum class EJSSpurtComboType : uint8_t
{
	None = 0,
	CW = 1,
	WC = 2,
	CWW = 3,
	WCW = 4,
	WCWW = 5,
	BestCWW = 6,
	EJSSpurtComboType_MAX = 7
};

// Object: Enum MoeGameCommonRuntime.EProjectGLookAtType
enum class EProjectGLookAtType : uint8_t
{
	None = 0,
	LookAtLocation = 1,
	LookAtTarget = 2,
	LookAtCamera = 3,
	EProjectGLookAtType_MAX = 4
};

// Object: Enum MoeGameCommonRuntime.EProjectGLookAtState
enum class EProjectGLookAtState : uint8_t
{
	None = 0,
	Rotating = 1,
	LookAting = 2,
	Reseting = 3,
	EProjectGLookAtState_MAX = 4
};

// Object: Enum MoeGameCommonRuntime.EPASettingReason
enum class EPASettingReason : uint8_t
{
	Init = 0,
	Idle = 1,
	Walk = 2,
	Attack = 3,
	Hit = 4,
	Ragdoll = 5,
	EPASettingReason_MAX = 6
};

// Object: Enum MoeGameCommonRuntime.EPABoneNamesType
enum class EPABoneNamesType : uint8_t
{
	Array = 0,
	LimbName = 1,
	All = 2,
	EPABoneNamesType_MAX = 3
};

// Object: Enum MoeGameCommonRuntime.EJSCameraState
enum class EJSCameraState : uint8_t
{
	Follow = 0,
	Waiting = 1,
	Pursue = 2,
	Restore = 3,
	EJSCameraState_MAX = 4
};

// Object: Enum MoeGameCommonRuntime.ECameraFovFadeReason
enum class ECameraFovFadeReason : uint8_t
{
	None = 0,
	Trigger = 1,
	AccBox = 2,
	SpeedUp = 4,
	SpeedUpProp = 8,
	ECameraFovFadeReason_MAX = 9
};

// Object: Enum MoeGameCommonRuntime.EUgcCharXRayType
enum class EUgcCharXRayType : uint16_t
{
	ShowEnemy = 0,
	ShowCamp = 1,
	ShowRandom = 2,
	InvalidType = 255,
	EUgcCharXRayType_MAX = 256
};

// Object: Enum MoeGameCommonRuntime.EXWInteractLockBpFlag
enum class EXWInteractLockBpFlag : uint8_t
{
	BeforeEnter = 0,
	BeforeExit = 1,
	DoOnEnter = 2,
	DoOnExit = 3,
	RemoveBuff = 4,
	SwitchCollision = 5,
	StateChangeAttribute = 6,
	RestoreCharScale = 7,
	EXWInteractLockBpFlag_MAX = 8
};

// Package: PhysicsControl
// Enums: 2

// Object: Enum PhysicsControl.EPhysicsControlType
enum class EPhysicsControlType : uint8_t
{
	WorldSpace = 0,
	ParentSpace = 1,
	EPhysicsControlType_MAX = 2
};

// Object: Enum PhysicsControl.EPhysicsMovementType
enum class EPhysicsMovementType : uint8_t
{
	Static = 0,
	Kinematic = 1,
	Simulated = 2,
	PAKinematic = 3,
	Ignore = 4,
	EPhysicsMovementType_MAX = 5
};

// Package: RPGCommon
// Enums: 2

// Object: Enum RPGCommon.EMoeRPGRepDataType
enum class EMoeRPGRepDataType : uint8_t
{
	INT = 1,
	FLOAT = 5,
	BOOL = 7,
	BOOLWITHTIMESTAMP = 8,
	EMoeRPGRepDataType_MAX = 9
};

// Object: Enum RPGCommon.EMoeRPGUnionType
enum class EMoeRPGUnionType : uint8_t
{
	InValid = 0,
	INT = 1,
	FLOAT = 2,
	BOOL = 3,
	EMoeRPGUnionType_MAX = 4
};

// Package: AbleCore
// Enums: 8

// Object: Enum AbleCore.EAblAbilityTaskResult
enum class EAblAbilityTaskResult : uint8_t
{
	Successful = 0,
	Branched = 1,
	Interrupted = 2,
	Canceled = 3,
	EAblAbilityTaskResult_MAX = 4
};

// Object: Enum AbleCore.EAblAbilityPassiveBehavior
enum class EAblAbilityPassiveBehavior : uint8_t
{
	CannotBePassive = 0,
	RefreshDuration = 1,
	IncreaseStackCount = 2,
	IncreaseAndRefresh = 3,
	EAblAbilityPassiveBehavior_MAX = 4
};

// Object: Enum AbleCore.EAblAbilityStartResult
enum class EAblAbilityStartResult : uint8_t
{
	InvalidTarget = 0,
	FailedCustomCheck = 1,
	CooldownNotExpired = 2,
	CannotInterruptCurrentAbility = 3,
	NotAllowedAsPassive = 4,
	PassiveMaxStacksReached = 5,
	InternalSystemsError = 6,
	AsyncProcessing = 7,
	ForwardedToServer = 8,
	InvalidParameter = 9,
	Success = 10,
	EAblAbilityStartResult_MAX = 11
};

// Object: Enum AbleCore.EAblAbilityTargetType
enum class EAblAbilityTargetType : uint8_t
{
	Self = 0,
	Owner = 1,
	Instigator = 2,
	TargetActor = 3,
	Camera = 4,
	EAblAbilityTargetType_MAX = 5
};

// Object: Enum AbleCore.EAblActionBranch
enum class EAblActionBranch : uint16_t
{
	Both = 0,
	OnTrue = 1,
	OnFalse = 2,
	None = 255,
	EAblActionBranch_MAX = 256
};

// Object: Enum AbleCore.EAblTaskKind
enum class EAblTaskKind : uint8_t
{
	Event = 0,
	Condition = 1,
	Action = 2,
	EAblTaskKind_MAX = 3
};

// Object: Enum AbleCore.EAblTaskExportType
enum class EAblTaskExportType : uint8_t
{
	Gameplay = 0,
	Effect = 1,
	Sound = 2,
	Max = 3
};

// Object: Enum AbleCore.EAblAbilityTaskRealm
enum class EAblAbilityTaskRealm : uint8_t
{
	Client = 0,
	Server = 1,
	ClientAndServer = 2,
	TotalRealms = 3,
	EAblAbilityTaskRealm_MAX = 4
};

// Package: Gamelet
// Enums: 2

// Object: Enum Gamelet.EGameletEnvironment
enum class EGameletEnvironment : uint8_t
{
	Gamelet_Test = 0,
	Gamelet_Product = 1,
	Gamelet_SingaporeTest = 2,
	Gamelet_SingaporeProduct = 3,
	Gamelet_CodingTEST = 4,
	Gamelet_CodingPRODUCT = 5,
	Gamelet_MAX = 6
};

// Object: Enum Gamelet.ECmd
enum class ECmd : uint32_t
{
	ECmdGSendMessageToApp = 10001,
	ECmdS2ESDKInitialized = 40001,
	ECmdS2EOnCgiProcessComplete = 40002,
	ECmdS2EStartPuertsVM = 40003,
	ECmdS2EPrepareGcrpRes = 40004,
	ECmdS2PMin = 60000,
	ECmdS2PMax = 69999,
	ECmd_MAX = 70000
};

// Package: PixUI
// Enums: 11

// Object: Enum PixUI.EPxKeyboardTypes
enum class EPxKeyboardTypes : uint8_t
{
	Default = 0,
	Number = 1,
	Password = 2,
	MultiLine = 3,
	EPxKeyboardTypes_MAX = 4
};

// Object: Enum PixUI.EPxWidgetTransformType
enum class EPxWidgetTransformType : uint8_t
{
	MoveBy = 0,
	MoveTo = 1,
	ResizeBy = 2,
	ResizeTo = 3,
	ScrollBy = 4,
	ScrollTo = 5,
	Count = 6,
	EPxWidgetTransformType_MAX = 7
};

// Object: Enum PixUI.EPxTickMode
enum class EPxTickMode : uint8_t
{
	JS = 0,
	Layout = 1,
	All = 2,
	EPxTickMode_MAX = 3
};

// Object: Enum PixUI.EPxFontFaceType
enum class EPxFontFaceType : uint8_t
{
	FaceType100 = 0,
	FaceType200 = 1,
	FaceType300 = 2,
	FaceType350 = 3,
	FaceType400 = 4,
	FaceType500 = 5,
	FaceType600 = 6,
	FaceType700 = 7,
	FaceType800 = 8,
	FaceType900 = 9,
	FaceType950 = 10,
	EPxFontFaceType_MAX = 11
};

// Object: Enum PixUI.EPxDynamicTextureUpdateMode
enum class EPxDynamicTextureUpdateMode : uint8_t
{
	UpdateRegion = 0,
	UpdateBulkData = 1,
	EPxDynamicTextureUpdateMode_MAX = 2
};

// Object: Enum PixUI.EPxKeyEventType
enum class EPxKeyEventType : uint8_t
{
	KeyDown = 0,
	KeyUp = 1,
	EPxKeyEventType_MAX = 2
};

// Object: Enum PixUI.EPxMouseType
enum class EPxMouseType : uint8_t
{
	MouseLeft = 0,
	MouseRight = 1,
	MouseMiddle = 2,
	MouseThumb = 3,
	MouseThumb2 = 4,
	MouseUnknown = 5,
	EPxMouseType_MAX = 6
};

// Object: Enum PixUI.EPxTouchType
enum class EPxTouchType : uint8_t
{
	TouchStart = 0,
	TouchMoved = 1,
	TouchEnd = 2,
	TouchCanceled = 3,
	EPxTouchType_MAX = 4
};

// Object: Enum PixUI.EPxPublicCapability
enum class EPxPublicCapability : uint8_t
{
	HookCoreLibProfiler = 0,
	AsyncModeUseSemaphore = 1,
	DelayFreePxImgBrush = 2,
	AllowBigFontSize = 3,
	ImageBlendsInLinearSpace = 4,
	ForceShapeMeasureText = 5,
	Count = 6,
	EPxPublicCapability_MAX = 7
};

// Object: Enum PixUI.EPxDebugInfo
enum class EPxDebugInfo : uint8_t
{
	ShowMouse = 0,
	Count = 1,
	EPxDebugInfo_MAX = 2
};

// Object: Enum PixUI.EPxWidgetBatchType
enum class EPxWidgetBatchType : uint8_t
{
	Default = 0,
	Auto = 1,
	NoBatch = 2,
	EPxWidgetBatchType_MAX = 3
};

// Package: PixUILog
// Enums: 2

// Object: Enum PixUILog.EPxLogLevels
enum class EPxLogLevels : uint8_t
{
	Log = 0,
	Warning = 1,
	Error = 2,
	Debug = 3,
	Count = 4,
	EPxLogLevels_MAX = 5
};

// Object: Enum PixUILog.EPxLogGroups
enum class EPxLogGroups : uint8_t
{
	Core = 0,
	Plugin = 1,
	Script = 2,
	Profiler = 3,
	Count = 4,
	EPxLogGroups_MAX = 5
};

// Package: PixUIProfiler
// Enums: 1

// Object: Enum PixUIProfiler.EPxProfilerCapability
enum class EPxProfilerCapability : uint8_t
{
	PrintSizeChange = 0,
	PrintCountChange = 1,
	AsyncInnerData = 2,
	Count = 3,
	EPxProfilerCapability_MAX = 4
};

// Package: RuntimeArchiver
// Enums: 6

// Object: Enum RuntimeArchiver.ERuntimeArchiverErrorCode
enum class ERuntimeArchiverErrorCode : uint8_t
{
	NotInitialized = 0,
	UnsupportedMode = 1,
	UnsupportedLocation = 2,
	ExtractError = 3,
	AddError = 4,
	CloseError = 5,
	GetError = 6,
	InvalidArgument = 7,
	ERuntimeArchiverErrorCode_MAX = 8
};

// Object: Enum RuntimeArchiver.EArchiveType
enum class EArchiveType : uint8_t
{
	DEFAULT = 0,
	ZIP = 1,
	UNZIP = 2,
	EArchiveType_MAX = 3
};

// Object: Enum RuntimeArchiver.ERuntimeArchiverRawFormat
enum class ERuntimeArchiverRawFormat : uint8_t
{
	Oodle = 0,
	GZip = 1,
	LZ4 = 2,
	ERuntimeArchiverRawFormat_MAX = 3
};

// Object: Enum RuntimeArchiver.ERuntimeArchiverLocation
enum class ERuntimeArchiverLocation : uint8_t
{
	Undefined = 0,
	Storage = 1,
	Memory = 2,
	ERuntimeArchiverLocation_MAX = 3
};

// Object: Enum RuntimeArchiver.ERuntimeArchiverMode
enum class ERuntimeArchiverMode : uint8_t
{
	Undefined = 0,
	Read = 1,
	Write = 2,
	ERuntimeArchiverMode_MAX = 3
};

// Object: Enum RuntimeArchiver.ERuntimeArchiverCompressionLevel
enum class ERuntimeArchiverCompressionLevel : uint8_t
{
	Compression0 = 0,
	Compression1 = 1,
	Compression2 = 2,
	Compression3 = 3,
	Compression4 = 4,
	Compression5 = 5,
	Compression6 = 6,
	Compression7 = 7,
	Compression8 = 8,
	Compression9 = 9,
	Compression10 = 10,
	ERuntimeArchiverCompressionLevel_MAX = 11
};

// Package: MoeAssetLoadAnalysis
// Enums: 2

// Object: Enum MoeAssetLoadAnalysis.ELGProfileLogType
enum class ELGProfileLogType : uint8_t
{
	ProfileLogType_AssetLoaded = 0,
	ProfileLogType_UObjectLoaded = 1,
	ProfileLogType_MAX = 2
};

// Object: Enum MoeAssetLoadAnalysis.EUnloadStrategy
enum class EUnloadStrategy : uint8_t
{
	KeepLoaded = 0,
	UnloadOnLevelExit = 1,
	UnloadAfterDelay = 2,
	EUnloadStrategy_MAX = 3
};

// Package: ZipIt
// Enums: 2

// Object: Enum ZipIt.EZipCreationFlag
enum class EZipCreationFlag : uint8_t
{
	CancelIfExists = 0,
	Overwrite = 1,
	Append = 2,
	EZipCreationFlag_MAX = 3
};

// Object: Enum ZipIt.EZipCompressLevel
enum class EZipCompressLevel : uint8_t
{
	Level0 = 0,
	Level1 = 1,
	Level2 = 2,
	Level3 = 3,
	Level4 = 4,
	Level5 = 5,
	Level6 = 6,
	Level7 = 7,
	Level8 = 8,
	Level9 = 9,
	EZipCompressLevel_MAX = 10
};

// Package: ACLPlugin
// Enums: 5

// Object: Enum ACLPlugin.ACLCompressionLevel
enum class EACLCompressionLevel : uint8_t
{
	ACLCL_Lowest = 0,
	ACLCL_Low = 1,
	ACLCL_Medium = 2,
	ACLCL_High = 3,
	ACLCL_Highest = 4,
	ACLCL_MAX = 5
};

// Object: Enum ACLPlugin.ACLVectorFormat
enum class EACLVectorFormat : uint8_t
{
	ACLVF_Vector3 = 0,
	ACLVF_Vector3_Variable = 1,
	ACLVF_Vector3_MAX = 2
};

// Object: Enum ACLPlugin.ACLRotationFormat
enum class EACLRotationFormat : uint8_t
{
	ACLRF_Quat = 0,
	ACLRF_QuatDropW = 1,
	ACLRF_QuatDropW_Variable = 2,
	ACLRF_MAX = 3
};

// Object: Enum ACLPlugin.ACLVisualFidelityChangeResult
enum class EACLVisualFidelityChangeResult : uint8_t
{
	Dispatched = 0,
	Completed = 1,
	Failed = 2,
	ACLVisualFidelityChangeResult_MAX = 3
};

// Object: Enum ACLPlugin.ACLVisualFidelity
enum class EACLVisualFidelity : uint8_t
{
	Highest = 0,
	Medium = 1,
	Lowest = 2,
	ACLVisualFidelity_MAX = 3
};

// Package: ControlRig
// Enums: 37

// Object: Enum ControlRig.EControlRigComponentMapDirection
enum class EControlRigComponentMapDirection : uint8_t
{
	Input = 0,
	Output = 1,
	EControlRigComponentMapDirection_MAX = 2
};

// Object: Enum ControlRig.EControlRigComponentSpace
enum class EControlRigComponentSpace : uint8_t
{
	WorldSpace = 0,
	ActorSpace = 1,
	ComponentSpace = 2,
	RigSpace = 3,
	LocalSpace = 4,
	Max = 5
};

// Object: Enum ControlRig.ERigExecutionType
enum class ERigExecutionType : uint8_t
{
	Runtime = 0,
	Editing = 1,
	Max = 2
};

// Object: Enum ControlRig.EBoneGetterSetterMode
enum class EBoneGetterSetterMode : uint8_t
{
	LocalSpace = 0,
	GlobalSpace = 1,
	Max = 2
};

// Object: Enum ControlRig.ETransformGetterType
enum class ETransformGetterType : uint8_t
{
	Initial = 0,
	Current = 1,
	Max = 2
};

// Object: Enum ControlRig.EControlRigClampSpatialMode
enum class EControlRigClampSpatialMode : uint8_t
{
	Plane = 0,
	Cylinder = 1,
	Sphere = 2,
	EControlRigClampSpatialMode_MAX = 3
};

// Object: Enum ControlRig.ETransformSpaceMode
enum class ETransformSpaceMode : uint8_t
{
	LocalSpace = 0,
	GlobalSpace = 1,
	BaseSpace = 2,
	BaseJoint = 3,
	Max = 4
};

// Object: Enum ControlRig.EControlRigDrawSettings
enum class EControlRigDrawSettings : uint8_t
{
	Points = 0,
	Lines = 1,
	LineStrip = 2,
	DynamicMesh = 3,
	EControlRigDrawSettings_MAX = 4
};

// Object: Enum ControlRig.EControlRigDrawHierarchyMode
enum class EControlRigDrawHierarchyMode : uint8_t
{
	Axes = 0,
	Max = 1
};

// Object: Enum ControlRig.EControlRigAnimEasingType
enum class EControlRigAnimEasingType : uint8_t
{
	Linear = 0,
	QuadraticEaseIn = 1,
	QuadraticEaseOut = 2,
	QuadraticEaseInOut = 3,
	CubicEaseIn = 4,
	CubicEaseOut = 5,
	CubicEaseInOut = 6,
	QuarticEaseIn = 7,
	QuarticEaseOut = 8,
	QuarticEaseInOut = 9,
	QuinticEaseIn = 10,
	QuinticEaseOut = 11,
	QuinticEaseInOut = 12,
	SineEaseIn = 13,
	SineEaseOut = 14,
	SineEaseInOut = 15,
	CircularEaseIn = 16,
	CircularEaseOut = 17,
	CircularEaseInOut = 18,
	ExponentialEaseIn = 19,
	ExponentialEaseOut = 20,
	ExponentialEaseInOut = 21,
	ElasticEaseIn = 22,
	ElasticEaseOut = 23,
	ElasticEaseInOut = 24,
	BackEaseIn = 25,
	BackEaseOut = 26,
	BackEaseInOut = 27,
	BounceEaseIn = 28,
	BounceEaseOut = 29,
	BounceEaseInOut = 30,
	EControlRigAnimEasingType_MAX = 31
};

// Object: Enum ControlRig.EControlRigRotationOrder
enum class EControlRigRotationOrder : uint8_t
{
	XYZ = 0,
	XZY = 1,
	YXZ = 2,
	YZX = 3,
	ZXY = 4,
	ZYX = 5,
	EControlRigRotationOrder_MAX = 6
};

// Object: Enum ControlRig.ECRSimPointIntegrateType
enum class ECRSimPointIntegrateType : uint8_t
{
	Verlet = 0,
	SemiExplicitEuler = 1,
	ECRSimPointIntegrateType_MAX = 2
};

// Object: Enum ControlRig.ECRSimConstraintType
enum class ECRSimConstraintType : uint8_t
{
	Distance = 0,
	DistanceFromA = 1,
	DistanceFromB = 2,
	Plane = 3,
	ECRSimConstraintType_MAX = 4
};

// Object: Enum ControlRig.ECRSimPointForceType
enum class ECRSimPointForceType : uint8_t
{
	Direction = 0,
	ECRSimPointForceType_MAX = 1
};

// Object: Enum ControlRig.ECRSimSoftCollisionType
enum class ECRSimSoftCollisionType : uint8_t
{
	Plane = 0,
	Sphere = 1,
	Cone = 2,
	ECRSimSoftCollisionType_MAX = 3
};

// Object: Enum ControlRig.EControlRigFKRigExecuteMode
enum class EControlRigFKRigExecuteMode : uint8_t
{
	Replace = 0,
	Additive = 1,
	Max = 2
};

// Object: Enum ControlRig.ERigBoneType
enum class ERigBoneType : uint8_t
{
	Imported = 0,
	User = 1,
	ERigBoneType_MAX = 2
};

// Object: Enum ControlRig.ERigControlAxis
enum class ERigControlAxis : uint8_t
{
	X = 0,
	Y = 1,
	Z = 2,
	ERigControlAxis_MAX = 3
};

// Object: Enum ControlRig.ERigControlValueType
enum class ERigControlValueType : uint8_t
{
	Initial = 0,
	Current = 1,
	Minimum = 2,
	Maximum = 3,
	ERigControlValueType_MAX = 4
};

// Object: Enum ControlRig.ERigControlType
enum class ERigControlType : uint8_t
{
	Bool = 0,
	Float = 1,
	Integer = 2,
	Vector2D = 3,
	Position = 4,
	Scale = 5,
	Rotator = 6,
	Transform = 7,
	TransformNoScale = 8,
	EulerTransform = 9,
	ERigControlType_MAX = 10
};

// Object: Enum ControlRig.ERigHierarchyImportMode
enum class ERigHierarchyImportMode : uint8_t
{
	Append = 0,
	Replace = 1,
	ReplaceLocalTransform = 2,
	ReplaceGlobalTransform = 3,
	Max = 4
};

// Object: Enum ControlRig.EControlRigSetKey
enum class EControlRigSetKey : uint8_t
{
	DoNotCare = 0,
	Always = 1,
	Never = 2,
	EControlRigSetKey_MAX = 3
};

// Object: Enum ControlRig.ERigEvent
enum class ERigEvent : uint8_t
{
	None = 0,
	RequestAutoKey = 1,
	Max = 2
};

// Object: Enum ControlRig.ERigElementType
enum class ERigElementType : uint8_t
{
	None = 0,
	Bone = 1,
	Space = 2,
	Control = 4,
	Curve = 8,
	All = 15,
	ERigElementType_MAX = 16
};

// Object: Enum ControlRig.ERigSpaceType
enum class ERigSpaceType : uint8_t
{
	Global = 0,
	Bone = 1,
	Control = 2,
	Space = 3,
	ERigSpaceType_MAX = 4
};

// Object: Enum ControlRig.EAimMode
enum class EAimMode : uint8_t
{
	AimAtTarget = 0,
	OrientToTarget = 1,
	MAX = 2
};

// Object: Enum ControlRig.EApplyTransformMode
enum class EApplyTransformMode : uint8_t
{
	Override = 0,
	Additive = 1,
	Max = 2
};

// Object: Enum ControlRig.ERigUnitDebugPointMode
enum class ERigUnitDebugPointMode : uint8_t
{
	Point = 0,
	Vector = 1,
	Max = 2
};

// Object: Enum ControlRig.ERigUnitDebugTransformMode
enum class ERigUnitDebugTransformMode : uint8_t
{
	Point = 0,
	Axes = 1,
	Box = 2,
	Max = 3
};

// Object: Enum ControlRig.EControlRigCurveAlignment
enum class EControlRigCurveAlignment : uint8_t
{
	Front = 0,
	Stretched = 1,
	EControlRigCurveAlignment_MAX = 2
};

// Object: Enum ControlRig.EControlRigVectorKind
enum class EControlRigVectorKind : uint8_t
{
	Direction = 0,
	Location = 1,
	EControlRigVectorKind_MAX = 2
};

// Object: Enum ControlRig.ERBFVectorDistanceType
enum class ERBFVectorDistanceType : uint8_t
{
	Euclidean = 0,
	Manhattan = 1,
	ArcLength = 2,
	ERBFVectorDistanceType_MAX = 3
};

// Object: Enum ControlRig.ERBFQuatDistanceType
enum class ERBFQuatDistanceType : uint8_t
{
	Euclidean = 0,
	ArcLength = 1,
	SwingAngle = 2,
	TwistAngle = 3,
	ERBFQuatDistanceType_MAX = 4
};

// Object: Enum ControlRig.ERBFKernelType
enum class ERBFKernelType : uint8_t
{
	Gaussian = 0,
	Exponential = 1,
	Linear = 2,
	Cubic = 3,
	Quintic = 4,
	ERBFKernelType_MAX = 5
};

// Object: Enum ControlRig.EControlRigModifyBoneMode
enum class EControlRigModifyBoneMode : uint8_t
{
	OverrideLocal = 0,
	OverrideGlobal = 1,
	AdditiveLocal = 2,
	AdditiveGlobal = 3,
	Max = 4
};

// Object: Enum ControlRig.ERigUnitVisualDebugPointMode
enum class ERigUnitVisualDebugPointMode : uint8_t
{
	Point = 0,
	Vector = 1,
	Max = 2
};

// Object: Enum ControlRig.EControlRigState
enum class EControlRigState : uint8_t
{
	Init = 0,
	Update = 1,
	Invalid = 2,
	EControlRigState_MAX = 3
};

// Package: AnimGraphRuntime
// Enums: 28

// Object: Enum AnimGraphRuntime.ESphericalLimitType
enum class ESphericalLimitType : uint8_t
{
	Inner = 0,
	Outer = 1,
	ESphericalLimitType_MAX = 2
};

// Object: Enum AnimGraphRuntime.AnimPhysSimSpaceType
enum class EAnimPhysSimSpaceType : uint8_t
{
	Component = 0,
	Actor = 1,
	World = 2,
	RootRelative = 3,
	BoneRelative = 4,
	AnimPhysSimSpaceType_MAX = 5
};

// Object: Enum AnimGraphRuntime.AnimPhysLinearConstraintType
enum class EAnimPhysLinearConstraintType : uint8_t
{
	Free = 0,
	Limited = 1,
	AnimPhysLinearConstraintType_MAX = 2
};

// Object: Enum AnimGraphRuntime.AnimPhysAngularConstraintType
enum class EAnimPhysAngularConstraintType : uint8_t
{
	Angular = 0,
	Cone = 1,
	AnimPhysAngularConstraintType_MAX = 2
};

// Object: Enum AnimGraphRuntime.EBlendListTransitionType
enum class EBlendListTransitionType : uint8_t
{
	StandardBlend = 0,
	Inertialization = 1,
	EBlendListTransitionType_MAX = 2
};

// Object: Enum AnimGraphRuntime.EDrivenDestinationMode
enum class EDrivenDestinationMode : uint8_t
{
	Bone = 0,
	MorphTarget = 1,
	MaterialParameter = 2,
	EDrivenDestinationMode_MAX = 3
};

// Object: Enum AnimGraphRuntime.EDrivenBoneModificationMode
enum class EDrivenBoneModificationMode : uint8_t
{
	AddToInput = 0,
	ReplaceComponent = 1,
	AddToRefPose = 2,
	EDrivenBoneModificationMode_MAX = 3
};

// Object: Enum AnimGraphRuntime.EConstraintOffsetOption
enum class EConstraintOffsetOption : uint8_t
{
	None = 0,
	Offset_RefPose = 1,
	EConstraintOffsetOption_MAX = 2
};

// Object: Enum AnimGraphRuntime.CopyBoneDeltaMode
enum class ECopyBoneDeltaMode : uint8_t
{
	Accumulate = 0,
	Copy = 1,
	CopyBoneDeltaMode_MAX = 2
};

// Object: Enum AnimGraphRuntime.EBlendCurveSource
enum class EBlendCurveSource : uint8_t
{
	Self = 0,
	Ancestor = 1,
	EBlendCurveSource_MAX = 2
};

// Object: Enum AnimGraphRuntime.EInterpolationBlend
enum class EInterpolationBlend : uint8_t
{
	Linear = 0,
	Cubic = 1,
	Sinusoidal = 2,
	EaseInOutExponent2 = 3,
	EaseInOutExponent3 = 4,
	EaseInOutExponent4 = 5,
	EaseInOutExponent5 = 6,
	MAX = 7
};

// Object: Enum AnimGraphRuntime.EBoneModificationMode
enum class EBoneModificationMode : uint8_t
{
	BMM_Ignore = 0,
	BMM_Replace = 1,
	BMM_Additive = 2,
	BMM_MAX = 3
};

// Object: Enum AnimGraphRuntime.EModifyCurveApplyMode
enum class EModifyCurveApplyMode : uint8_t
{
	Add = 0,
	Scale = 1,
	Blend = 2,
	WeightedMovingAverage = 3,
	RemapCurve = 4,
	EModifyCurveApplyMode_MAX = 5
};

// Object: Enum AnimGraphRuntime.EPoseDriverOutput
enum class EPoseDriverOutput : uint8_t
{
	DrivePoses = 0,
	DriveCurves = 1,
	EPoseDriverOutput_MAX = 2
};

// Object: Enum AnimGraphRuntime.EPoseDriverSource
enum class EPoseDriverSource : uint8_t
{
	Rotation = 0,
	Translation = 1,
	EPoseDriverSource_MAX = 2
};

// Object: Enum AnimGraphRuntime.EPoseDriverType
enum class EPoseDriverType : uint8_t
{
	SwingAndTwist = 0,
	SwingOnly = 1,
	Translation = 2,
	EPoseDriverType_MAX = 3
};

// Object: Enum AnimGraphRuntime.ESnapshotSourceMode
enum class ESnapshotSourceMode : uint8_t
{
	NamedSnapshot = 0,
	SnapshotPin = 1,
	ESnapshotSourceMode_MAX = 2
};

// Object: Enum AnimGraphRuntime.ERefPoseType
enum class ERefPoseType : uint8_t
{
	EIT_LocalSpace = 0,
	EIT_Additive = 1,
	EIT_MAX = 2
};

// Object: Enum AnimGraphRuntime.ESimulationSpace
enum class ESimulationSpace : uint8_t
{
	ComponentSpace = 0,
	WorldSpace = 1,
	BaseBoneSpace = 2,
	ESimulationSpace_MAX = 3
};

// Object: Enum AnimGraphRuntime.EScaleChainInitialLength
enum class EScaleChainInitialLength : uint8_t
{
	FixedDefaultLengthValue = 0,
	Distance = 1,
	ChainLength = 2,
	EScaleChainInitialLength_MAX = 3
};

// Object: Enum AnimGraphRuntime.ESequenceEvalReinit
enum class ESequenceEvalReinit : uint8_t
{
	NoReset = 0,
	StartPosition = 1,
	ExplicitTime = 2,
	ESequenceEvalReinit_MAX = 3
};

// Object: Enum AnimGraphRuntime.ESplineBoneAxis
enum class ESplineBoneAxis : uint8_t
{
	None = 0,
	X = 1,
	Y = 2,
	Z = 3,
	ESplineBoneAxis_MAX = 4
};

// Object: Enum AnimGraphRuntime.ERotationComponent
enum class ERotationComponent : uint8_t
{
	EulerX = 0,
	EulerY = 1,
	EulerZ = 2,
	QuaternionAngle = 3,
	SwingAngle = 4,
	TwistAngle = 5,
	ERotationComponent_MAX = 6
};

// Object: Enum AnimGraphRuntime.EEasingFuncType
enum class EEasingFuncType : uint8_t
{
	Linear = 0,
	Sinusoidal = 1,
	Cubic = 2,
	QuadraticInOut = 3,
	CubicInOut = 4,
	HermiteCubic = 5,
	QuarticInOut = 6,
	QuinticInOut = 7,
	CircularIn = 8,
	CircularOut = 9,
	CircularInOut = 10,
	ExpIn = 11,
	ExpOut = 12,
	ExpInOut = 13,
	CustomCurve = 14,
	EEasingFuncType_MAX = 15
};

// Object: Enum AnimGraphRuntime.ERBFNormalizeMethod
enum class ERBFNormalizeMethod : uint8_t
{
	OnlyNormalizeAboveOne = 0,
	AlwaysNormalize = 1,
	NormalizeWithinMedian = 2,
	NoNormalization = 3,
	ERBFNormalizeMethod_MAX = 4
};

// Object: Enum AnimGraphRuntime.ERBFDistanceMethod
enum class ERBFDistanceMethod : uint8_t
{
	Euclidean = 0,
	Quaternion = 1,
	SwingAngle = 2,
	TwistAngle = 3,
	DefaultMethod = 4,
	ERBFDistanceMethod_MAX = 5
};

// Object: Enum AnimGraphRuntime.ERBFFunctionType
enum class ERBFFunctionType : uint8_t
{
	Gaussian = 0,
	Exponential = 1,
	Linear = 2,
	Cubic = 3,
	Quintic = 4,
	DefaultFunction = 5,
	ERBFFunctionType_MAX = 6
};

// Object: Enum AnimGraphRuntime.ERBFSolverType
enum class ERBFSolverType : uint8_t
{
	Additive = 0,
	Interpolative = 1,
	ERBFSolverType_MAX = 2
};

// Package: MovieSceneTracks
// Enums: 4

// Object: Enum MovieSceneTracks.MovieScene3DPathSection_Axis
enum class EMovieScene3DPathSection_Axis : uint8_t
{
	X = 0,
	Y = 1,
	Z = 2,
	NEG_X = 3,
	NEG_Y = 4,
	NEG_Z = 5,
	MovieScene3DPathSection_MAX = 6
};

// Object: Enum MovieSceneTracks.EFireEventsAtPosition
enum class EFireEventsAtPosition : uint8_t
{
	AtStartOfEvaluation = 0,
	AtEndOfEvaluation = 1,
	AfterSpawn = 2,
	EFireEventsAtPosition_MAX = 3
};

// Object: Enum MovieSceneTracks.ELevelVisibility
enum class ELevelVisibility : uint8_t
{
	Visible = 0,
	Hidden = 1,
	ELevelVisibility_MAX = 2
};

// Object: Enum MovieSceneTracks.EParticleKey
enum class EParticleKey : uint8_t
{
	Activate = 0,
	Deactivate = 1,
	Trigger = 2,
	EParticleKey_MAX = 3
};

// Package: BlueprintAttributes
// Enums: 1

// Object: Enum BlueprintAttributes.EGBAClampingType
enum class EGBAClampingType : uint8_t
{
	None = 0,
	Float = 1,
	AttributeBased = 2,
	EGBAClampingType_MAX = 3
};

// Package: BudgetEverything
// Enums: 1

// Object: Enum BudgetEverything.EBESignificanceLevel
enum class EBESignificanceLevel : uint8_t
{
	Default = 0,
	Extreme = 1,
	Ultra = 2,
	High = 3,
	Medium = 4,
	Low = 5,
	OutsideExtreme = 6,
	OutsideUltra = 7,
	OutsideHigh = 8,
	OutsideMedium = 9,
	OutsideLow = 10,
	Max = 11
};

// Package: AbleCoreSP
// Enums: 29

// Object: Enum AbleCoreSP.ESPRelativeDirType
enum class ESPRelativeDirType : uint8_t
{
	Default = 1,
	SpawnToTargetDir = 2,
	SpawnToTargetPitchDirButForward = 3,
	SpawnToTargetSnapDir = 4,
	InitialOrientation = 5,
	SavedRotation = 6,
	ESPRelativeDirType_MAX = 7
};

// Object: Enum AbleCoreSP.EAbleAbilityTargetType
enum class EAbleAbilityTargetType : uint8_t
{
	ATT_Self = 0,
	ATT_Owner = 1,
	ATT_Instigator = 2,
	ATT_TargetActor = 3,
	ATT_Camera = 4,
	ATT_Location = 5,
	ATT_RandomLocations = 6,
	ATT_World = 7,
	ATT_Master = 8,
	ATT_CurrentPet = 9,
	ATT_MorphMaster = 10,
	ATT_MorphMonster = 11,
	ATT_MAX = 12
};

// Object: Enum AbleCoreSP.EAblePlayCameraShakeStopMode
enum class EAblePlayCameraShakeStopMode : uint8_t
{
	DontStop = 0,
	Stop = 1,
	StopImmediately = 2,
	EAblePlayCameraShakeStopMode_MAX = 3
};

// Object: Enum AbleCoreSP.EAbleAbilityTaskResult
enum class EAbleAbilityTaskResult : uint8_t
{
	Successful = 0,
	Branched = 1,
	Interrupted = 2,
	Decayed = 3,
	BranchSegment = 4,
	StopAcross = 5,
	EAbleAbilityTaskResult_MAX = 6
};

// Object: Enum AbleCoreSP.EAbleClientExecutionPolicy
enum class EAbleClientExecutionPolicy : uint8_t
{
	Default = 0,
	LocalAndAuthoritativeOnly = 1,
	EAbleClientExecutionPolicy_MAX = 2
};

// Object: Enum AbleCoreSP.EAbleInstancePolicy
enum class EAbleInstancePolicy : uint8_t
{
	Default = 0,
	NewInstance = 1,
	NewInstanceReplicated = 2,
	EAbleInstancePolicy_MAX = 3
};

// Object: Enum AbleCoreSP.EAbleCallbackResult
enum class EAbleCallbackResult : uint8_t
{
	Complete = 0,
	KeepProcessing = 1,
	IgnoreActors = 2,
	EAbleCallbackResult_MAX = 3
};

// Object: Enum AbleCoreSP.EAbleAbilityPassiveBehavior
enum class EAbleAbilityPassiveBehavior : uint8_t
{
	CannotBePassive = 0,
	RefreshDuration = 1,
	IncreaseStackCount = 2,
	IncreaseAndRefresh = 3,
	EAbleAbilityPassiveBehavior_MAX = 4
};

// Object: Enum AbleCoreSP.EAbleAbilityStartResult
enum class EAbleAbilityStartResult : uint8_t
{
	InvalidTarget = 0,
	FailedCustomCheck = 1,
	CooldownNotExpired = 2,
	CannotInterruptCurrentAbility = 3,
	NotAllowedAsPassive = 4,
	PassiveMaxStacksReached = 5,
	InternalSystemsError = 6,
	AsyncProcessing = 7,
	ForwardedToServer = 8,
	InvalidParameter = 9,
	Success = 10,
	EAbleAbilityStartResult_MAX = 11
};

// Object: Enum AbleCoreSP.EStopAbilityType
enum class EStopAbilityType : uint8_t
{
	None = 0,
	Interrupt = 1,
	Finish = 2,
	EStopAbilityType_MAX = 3
};

// Object: Enum AbleCoreSP.EAbleChannelPresent
enum class EAbleChannelPresent : uint8_t
{
	ACP_Default = 0,
	ACP_Soc = 1,
	ACP_PlaceHolder1 = 2,
	ACP_PlaceHolder2 = 3,
	ACP_PlaceHolder3 = 4,
	ACP_Character = 5,
	ACP_PlaceHolder5 = 6,
	ACP_NonePresent = 7,
	ACP_MAX = 8
};

// Object: Enum AbleCoreSP.ESPAbleTraceType
enum class ESPAbleTraceType : uint8_t
{
	AT_None = 0,
	AT_Gameplay = 1,
	AT_Weapon = 2,
	AT_Pawn = 3,
	AT_WorldStatic = 4,
	AT_Floor = 5,
	AT_Climbable = 6,
	AT_CaptureBall = 7,
	AT_CaptureTrace = 8,
	AT_CallMonster = 9,
	AT_WildMonster = 10,
	AT_GameBuilding = 11,
	AT_GameProps = 12,
	AT_Choppable = 13,
	AT_BlockBuilding = 14,
	AT_ActualMesh = 15,
	AT_PackageAirWall = 16,
	AT_InterWater = 17,
	AT_AirWall = 18,
	AT_GameStaticBuilding = 19,
	AT_CallMonsterMesh = 20,
	AT_WildMonsterMesh = 21,
	AT_OWBossAirWall = 22,
	AT_MAX = 23
};

// Object: Enum AbleCoreSP.EAbleConditionResults
enum class EAbleConditionResults : uint8_t
{
	ACR_Passed = 0,
	ACR_Failed = 1,
	ACR_Ignored = 2,
	ACR_MAX = 3
};

// Object: Enum AbleCoreSP.EAbleAbilityTaskRealm
enum class EAbleAbilityTaskRealm : uint8_t
{
	ATR_Client = 0,
	ATR_Server = 1,
	ATR_ClientAndServer = 2,
	ATR_TotalRealms = 3,
	ATR_MAX = 4
};

// Object: Enum AbleCoreSP.EAbilityDependencyUsage
enum class EAbilityDependencyUsage : uint8_t
{
	Unknown = 0,
	Animation = 1,
	AnimationAdaptedBodyType = 2,
	Particle = 3,
	SoundBank = 4,
	Projectile = 5,
	Material = 6,
	PlaceHolder3 = 7,
	PlaceHolder4 = 8,
	PlaceHolder5 = 9,
	PlaceHolder6 = 10,
	Max = 99
};

// Object: Enum AbleCoreSP.EAbilityCollisionShape
enum class EAbilityCollisionShape : uint8_t
{
	Box = 0,
	Sphere = 1,
	Capsule = 2,
	Cone = 3,
	Cylinder = 4,
	EAbilityCollisionShape_MAX = 5
};

// Object: Enum AbleCoreSP.EAbleCancelAbilityPassiveBehavior
enum class EAbleCancelAbilityPassiveBehavior : uint8_t
{
	RemoveOneStack = 0,
	RemoveOneStackWithRefresh = 1,
	RemoveEntireStack = 2,
	EAbleCancelAbilityPassiveBehavior_MAX = 3
};

// Object: Enum AbleCoreSP.EAbleCollisionFilterSort
enum class EAbleCollisionFilterSort : uint8_t
{
	AbleFitlerSort_Ascending = 0,
	AbleFilterSort_Descending = 1,
	EAbleCollisionFilterSort_MAX = 2
};

// Object: Enum AbleCoreSP.EAbleJumpToTarget
enum class EAbleJumpToTarget : uint8_t
{
	JTT_Actor = 0,
	JTT_Location = 1,
	JTT_MAX = 2
};

// Object: Enum AbleCoreSP.EAblePathFindingType
enum class EAblePathFindingType : uint8_t
{
	Regular = 0,
	Hierarchical = 1,
	EAblePathFindingType_MAX = 2
};

// Object: Enum AbleCoreSP.EAbleMoveToTarget
enum class EAbleMoveToTarget : uint8_t
{
	MTT_Actor = 0,
	MTT_Location = 1,
	MTT_MAX = 2
};

// Object: Enum AbleCoreSP.EAblePlayAnimationTaskAnimMode
enum class EAblePlayAnimationTaskAnimMode : uint8_t
{
	SingleNode = 0,
	AbilityAnimationNode = 1,
	DynamicMontage = 2,
	EAblePlayAnimationTaskAnimMode_MAX = 3
};

// Object: Enum AbleCoreSP.ESPScaledBySummonType
enum class ESPScaledBySummonType : uint8_t
{
	None = 0,
	BySummonActor = 1,
	BySummonTarget = 2,
	BySummonAttachedTarget = 3,
	ESPScaledBySummonType_MAX = 4
};

// Object: Enum AbleCoreSP.ESPAttachComp
enum class ESPAttachComp : uint8_t
{
	None = 0,
	Skeletal = 1,
	Root = 2,
	Custom = 3,
	ESPAttachComp_MAX = 4
};

// Object: Enum AbleCoreSP.ESPPlayFallingType
enum class ESPPlayFallingType : uint8_t
{
	None = 0,
	Hidden = 1,
	ESPPlayFallingType_MAX = 2
};

// Object: Enum AbleCoreSP.EAbleTargetingFilterSort
enum class EAbleTargetingFilterSort : uint8_t
{
	AbleTargetFilterSort_Ascending = 0,
	AbleTargetFilterSort_Descending = 1,
	AbleTargetFilterSort_MAX = 2
};

// Object: Enum AbleCoreSP.ELineTraceCentricDestinationTerminalAxisFixMode
enum class ELineTraceCentricDestinationTerminalAxisFixMode : uint8_t
{
	FixMode_X = 0,
	FixMode_Y = 1,
	FixMode_Z = 2,
	FixMode_MAX = 3
};

// Object: Enum AbleCoreSP.ELineTraceCentricDestinationTerminalFixMode
enum class ELineTraceCentricDestinationTerminalFixMode : uint8_t
{
	TraceLineDirection = 0,
	NegativeTraceLineDirection = 1,
	HitNormalize = 2,
	WorldUp = 3,
	ELineTraceCentricDestinationTerminalFixMode_MAX = 4
};

// Object: Enum AbleCoreSP.EProvidableParameterMode
enum class EProvidableParameterMode : uint8_t
{
	MODE_CONST = 0,
	MODE_INSTANCED = 1,
	MODE_EXTERN = 2,
	MODE_MAX = 3
};

// Package: EnhancedInput
// Enums: 10

// Object: Enum EnhancedInput.EInputActionValueType
enum class EInputActionValueType : uint8_t
{
	Boolean = 0,
	Axis1D = 1,
	Axis2D = 2,
	Axis3D = 3,
	EInputActionValueType_MAX = 4
};

// Object: Enum EnhancedInput.EMappingQueryIssue
enum class EMappingQueryIssue : uint8_t
{
	NoIssue = 0,
	ReservedByAction = 1,
	HidesExistingMapping = 2,
	HiddenByExistingMapping = 4,
	CollisionWithMappingInSameContext = 8,
	ForcesTypePromotion = 16,
	ForcesTypeDemotion = 32,
	EMappingQueryIssue_MAX = 33
};

// Object: Enum EnhancedInput.EMappingQueryResult
enum class EMappingQueryResult : uint8_t
{
	Error_EnhancedInputNotEnabled = 0,
	Error_InputContextNotInActiveContexts = 1,
	NotMappable = 2,
	MappingAvailable = 3,
	EMappingQueryResult_MAX = 4
};

// Object: Enum EnhancedInput.EInputAxisSwizzle
enum class EInputAxisSwizzle : uint8_t
{
	YXZ = 0,
	ZYX = 1,
	XZY = 2,
	YZX = 3,
	ZXY = 4,
	EInputAxisSwizzle_MAX = 5
};

// Object: Enum EnhancedInput.EFOVScalingType
enum class EFOVScalingType : uint8_t
{
	Standard = 0,
	UE4_BackCompat = 1,
	EFOVScalingType_MAX = 2
};

// Object: Enum EnhancedInput.EDeadZoneType
enum class EDeadZoneType : uint8_t
{
	Axial = 0,
	Radial = 1,
	EDeadZoneType_MAX = 2
};

// Object: Enum EnhancedInput.EModifierExecutionPhase
enum class EModifierExecutionPhase : uint8_t
{
	PerInput = 0,
	FinalValue = 1,
	NumPhases = 2,
	EModifierExecutionPhase_MAX = 3
};

// Object: Enum EnhancedInput.ETriggerType
enum class ETriggerType : uint8_t
{
	Explicit = 0,
	Implicit = 1,
	Blocker = 2,
	ETriggerType_MAX = 3
};

// Object: Enum EnhancedInput.ETriggerEvent
enum class ETriggerEvent : uint8_t
{
	None = 0,
	Started = 1,
	Ongoing = 2,
	Canceled = 3,
	Triggered = 4,
	Completed = 5,
	ETriggerEvent_MAX = 6
};

// Object: Enum EnhancedInput.ETriggerState
enum class ETriggerState : uint8_t
{
	None = 0,
	Ongoing = 1,
	Triggered = 2,
	ETriggerState_MAX = 3
};

// Package: LiveLinkInterface
// Enums: 2

// Object: Enum LiveLinkInterface.ELiveLinkCameraProjectionMode
enum class ELiveLinkCameraProjectionMode : uint8_t
{
	Perspective = 0,
	Orthographic = 1,
	ELiveLinkCameraProjectionMode_MAX = 2
};

// Object: Enum LiveLinkInterface.ELiveLinkSourceMode
enum class ELiveLinkSourceMode : uint8_t
{
	Latest = 0,
	EngineTime = 1,
	Timecode = 2,
	ELiveLinkSourceMode_MAX = 3
};

// Package: LiveLink
// Enums: 2

// Object: Enum LiveLink.ELiveLinkAxis
enum class ELiveLinkAxis : uint8_t
{
	X = 0,
	Y = 1,
	Z = 2,
	XNeg = 3,
	YNeg = 4,
	ZNeg = 5,
	ELiveLinkAxis_MAX = 6
};

// Object: Enum LiveLink.ELiveLinkTimecodeProviderEvaluationType
enum class ELiveLinkTimecodeProviderEvaluationType : uint8_t
{
	Lerp = 0,
	Nearest = 1,
	Latest = 2,
	ELiveLinkTimecodeProviderEvaluationType_MAX = 3
};

// Package: TimeManagement
// Enums: 3

// Object: Enum TimeManagement.EFrameNumberDisplayFormats
enum class EFrameNumberDisplayFormats : uint8_t
{
	NonDropFrameTimecode = 0,
	DropFrameTimecode = 1,
	Seconds = 2,
	Frames = 3,
	MAX_Count = 4,
	EFrameNumberDisplayFormats_MAX = 5
};

// Object: Enum TimeManagement.ETimedDataInputState
enum class ETimedDataInputState : uint8_t
{
	Connected = 0,
	Unresponsive = 1,
	Disconnected = 2,
	ETimedDataInputState_MAX = 3
};

// Object: Enum TimeManagement.ETimedDataInputEvaluationType
enum class ETimedDataInputEvaluationType : uint8_t
{
	None = 0,
	Timecode = 1,
	PlatformTime = 2,
	ETimedDataInputEvaluationType_MAX = 3
};

// Package: RemoteControl
// Enums: 2

// Object: Enum RemoteControl.ERCAccess
enum class ERCAccess : uint8_t
{
	NO_ACCESS = 0,
	READ_ACCESS = 1,
	WRITE_ACCESS = 2,
	WRITE_TRANSACTION_ACCESS = 3,
	ERCAccess_MAX = 4
};

// Object: Enum RemoteControl.EExposedFieldType
enum class EExposedFieldType : uint8_t
{
	Invalid = 0,
	Property = 1,
	Function = 2,
	EExposedFieldType_MAX = 3
};

// Package: MoeHapticRuntime
// Enums: 1

// Object: Enum MoeHapticRuntime.EMoeHapticEventType
enum class EMoeHapticEventType : uint8_t
{
	Transient = 0,
	Continuous = 1,
	EMoeHapticEventType_MAX = 2
};

// Package: SGSRSpatialUpscaling
// Enums: 1

// Object: Enum SGSRSpatialUpscaling.ESGSRTarget
enum class ESGSRTarget : uint8_t
{
	SGSR_TARGET_MOBILE = 0,
	SGSR_TARGET_HIGH_QUALITY = 1,
	SGSR_TARGET_VR = 2,
	SGSR_MAX = 3
};

// Package: QQAvatar
// Enums: 6

// Object: Enum QQAvatar.EFaceSlotAdjustType
enum class EFaceSlotAdjustType : uint8_t
{
	FACE_SLOT_ADJUST_MOVE = 0,
	FACE_SLOT_ADJUST_SCALE = 1,
	FACE_SLOT_ADJUST_MAX = 2
};

// Object: Enum QQAvatar.ESameStyleContentType
enum class ESameStyleContentType : uint8_t
{
	SSCT_IMAGE = 0,
	SSCT_VIDEO = 1,
	SSCT_MAX = 2
};

// Object: Enum QQAvatar.ELestGoTestAvatarGrade
enum class ELestGoTestAvatarGrade : uint8_t
{
	EW_LOW = 0,
	EW_MID = 1,
	EW_HIGH = 2,
	EW_MAX = 3
};

// Object: Enum QQAvatar.EPieceType
enum class EPieceType : uint8_t
{
	PNormal = 0,
	PCloth = 1,
	PFace = 2,
	PEyeBrow = 3,
	PMax = 4,
	EPieceType_MAX = 5
};

// Object: Enum QQAvatar.ESPInputTouchType
enum class ESPInputTouchType : uint8_t
{
	SPINPUTTOUCH_BEGIN = 0,
	SPInputTouch_MOVED = 1,
	SPInputTouch_END = 2,
	ESPInputTouchType_MAX = 3
};

// Object: Enum QQAvatar.ESPPlatformType
enum class ESPPlatformType : uint8_t
{
	SP_WINDOWS = 0,
	SP_MAC = 1,
	SP_ANDROID = 2,
	SP_IOS = 3,
	SP_EDITOR = 4,
	SP_Other = 5,
	SP_MAX = 6
};

// Package: PakMountRuntime
// Enums: 1

// Object: Enum PakMountRuntime.MountReturnType
enum class EMountReturnType : uint8_t
{
	Failure = 0,
	NotExist = 1,
	Successed = 2,
	FailurePakPlatformFileIsNotValid = 3,
	FailurePakIntegrityError = 4,
	FailurePakEncryptionKeyIsNotValid = 5,
	FailurePakIndexIsCorrupted = 6,
	FailurePakZplanVersionIsNotCompatible = 7,
	MountReturnType_MAX = 8
};

// Package: KawaiiPhysics
// Enums: 6

// Object: Enum KawaiiPhysics.SurfaceColliderType
enum class ESurfaceColliderType : uint8_t
{
	Off = 0,
	Push = 1,
	PULL = 2,
	SurfaceColliderType_MAX = 3
};

// Object: Enum KawaiiPhysics.EXPBDComplianceType
enum class EXPBDComplianceType : uint8_t
{
	Concrete = 0,
	Wood = 1,
	Leather = 2,
	Tendon = 3,
	Rubber = 4,
	Muscle = 5,
	Fat = 6,
	EXPBDComplianceType_MAX = 7
};

// Object: Enum KawaiiPhysics.BoneConstraintMode
enum class EBoneConstraintMode : uint8_t
{
	PBD = 0,
	XPBD = 1,
	BoneConstraintMode_MAX = 2
};

// Object: Enum KawaiiPhysics.ECollisionLimitType
enum class ECollisionLimitType : uint8_t
{
	None = 0,
	Spherical = 1,
	Capsule = 2,
	Box = 3,
	Planar = 4,
	ECollisionLimitType_MAX = 5
};

// Object: Enum KawaiiPhysics.EBoneForwardAxis
enum class EBoneForwardAxis : uint8_t
{
	X_Positive = 0,
	X_Negative = 1,
	Y_Positive = 2,
	Y_Negative = 3,
	Z_Positive = 4,
	Z_Negative = 5,
	EBoneForwardAxis_MAX = 6
};

// Object: Enum KawaiiPhysics.EPlanarConstraint
enum class EPlanarConstraint : uint8_t
{
	None = 0,
	X = 1,
	Y = 2,
	Z = 3,
	EPlanarConstraint_MAX = 4
};

// Package: CMUIUX
// Enums: 9

// Object: Enum CMUIUX.ECMArrangeDirection
enum class ECMArrangeDirection : uint8_t
{
	Vertiacal = 0,
	Horizontal = 1,
	ECMArrangeDirection_MAX = 2
};

// Object: Enum CMUIUX.EStackOrientation
enum class EStackOrientation : uint8_t
{
	NONE = 0,
	HORIZONTAL = 1,
	VERTICAL = 2,
	RightToLeft = 3,
	BottomToTop = 4,
	EStackOrientation_MAX = 5
};

// Object: Enum CMUIUX.ECMToggleState
enum class ECMToggleState : uint8_t
{
	ToggleOn = 0,
	ToggleOff = 1,
	ECMToggleState_MAX = 2
};

// Object: Enum CMUIUX.CMSoundSourceType
enum class ECMSoundSourceType : uint8_t
{
	PRESET = 0,
	CUSTOM = 1,
	CMSoundSourceType_MAX = 2
};

// Object: Enum CMUIUX.CMSoundEvent
enum class ECMSoundEvent : uint8_t
{
	NONE = 0,
	CLICK = 1,
	DOWN = 2,
	UP = 3,
	CMSoundEvent_MAX = 4
};

// Object: Enum CMUIUX.ECMPanelLayer
enum class ECMPanelLayer : uint16_t
{
	Ground = 0,
	Popup = 2000,
	Top = 10000,
	NewbieGuide = 20000,
	TopMost = 30000,
	Debug = 35000,
	ECMPanelLayer_MAX = 35001
};

// Object: Enum CMUIUX.ECMProgressBarFillType
enum class ECMProgressBarFillType : uint8_t
{
	LeftToRight = 0,
	RightToLeft = 1,
	FillFromCenter = 2,
	TopToBottom = 3,
	BottomToTop = 4,
	Radians = 5,
	ECMProgressBarFillType_MAX = 6
};

// Object: Enum CMUIUX.ECMScrollWhenFocusChanges
enum class ECMScrollWhenFocusChanges : uint8_t
{
	NoScroll = 0,
	InstantScroll = 1,
	AnimatedScroll = 2,
	ECMScrollWhenFocusChanges_MAX = 3
};

// Object: Enum CMUIUX.ECMDescendantScrollDestination
enum class ECMDescendantScrollDestination : uint8_t
{
	IntoView = 0,
	TopOrLeft = 1,
	Center = 2,
	ECMDescendantScrollDestination_MAX = 3
};

// Package: CosHelper
// Enums: 1

// Object: Enum CosHelper.ECosHelperFileInfoType
enum class ECosHelperFileInfoType : uint8_t
{
	None = 0,
	ContentLength = 1,
	ETag = 2,
	LastModifiedUtcTimestamp = 4,
	ECosHelperFileInfoType_MAX = 5
};

// Package: HotPatcherRuntime
// Enums: 9

// Object: Enum HotPatcherRuntime.ETargetPlatform
enum class ETargetPlatform : uint8_t
{
	None = 0,
	AllPlatforms = 1,
	ETargetPlatform_MAX = 2
};

// Object: Enum HotPatcherRuntime.EMatchOperator
enum class EMatchOperator : uint8_t
{
	None = 0,
	GREAT_THAN = 1,
	LESS_THAN = 2,
	EQUAL = 3,
	EMatchOperator_MAX = 4
};

// Object: Enum HotPatcherRuntime.EMatchRule
enum class EMatchRule : uint8_t
{
	None = 0,
	MATCH = 1,
	IGNORE = 2,
	EMatchRule_MAX = 3
};

// Object: Enum HotPatcherRuntime.EMonolithicPathMode
enum class EMonolithicPathMode : uint8_t
{
	MountPath = 0,
	PackagePath = 1,
	EMonolithicPathMode_MAX = 2
};

// Object: Enum HotPatcherRuntime.EAssetRegistryRule
enum class EAssetRegistryRule : uint8_t
{
	PATCH = 0,
	PER_CHUNK = 1,
	CUSTOM = 2,
	EAssetRegistryRule_MAX = 3
};

// Object: Enum HotPatcherRuntime.EShaderLibNameRule
enum class EShaderLibNameRule : uint8_t
{
	VERSION_ID = 0,
	PROJECT_NAME = 1,
	CUSTOM = 2,
	EShaderLibNameRule_MAX = 3
};

// Object: Enum HotPatcherRuntime.EAssetRegistryDependencyTypeEx
enum class EAssetRegistryDependencyTypeEx : uint8_t
{
	None = 0,
	Soft = 1,
	Hard = 2,
	SearchableName = 3,
	SoftManage = 4,
	HardManage = 5,
	Packages = 6,
	Manage = 7,
	All = 8,
	EAssetRegistryDependencyTypeEx_MAX = 9
};

// Object: Enum HotPatcherRuntime.EPSOSaveMode
enum class EPSOSaveMode : uint8_t
{
	Incremental = 0,
	BoundPSOsOnly = 1,
	SortedBoundPSOs = 2,
	EPSOSaveMode_MAX = 3
};

// Object: Enum HotPatcherRuntime.ESearchCaseMode
enum class ESearchCaseMode : uint8_t
{
	CaseSensitive = 0,
	IgnoreCase = 1,
	ESearchCaseMode_MAX = 2
};

// Package: Pandora
// Enums: 2

// Object: Enum Pandora.EPropertyClass
enum class EPropertyClass : uint8_t
{
	Byte = 0,
	Int8 = 1,
	Int16 = 2,
	Int = 3,
	Int64 = 4,
	UInt16 = 5,
	UInt32 = 6,
	UInt64 = 7,
	UnsizedInt = 8,
	UnsizedUInt = 9,
	Float = 10,
	Double = 11,
	Bool = 12,
	SoftClass = 13,
	WeakObject = 14,
	LazyObject = 15,
	SoftObject = 16,
	Class = 17,
	Object = 18,
	Interface = 19,
	Name = 20,
	Str = 21,
	Array = 22,
	Map = 23,
	Set = 24,
	Struct = 25,
	Delegate = 26,
	MulticastDelegate = 27,
	Text = 28,
	Enum = 29,
	EPropertyClass_MAX = 30
};

// Object: Enum Pandora.EPandoraEnv
enum class EPandoraEnv : uint8_t
{
	Test = 0,
	Product = 1,
	EPandoraEnv_MAX = 2
};

// Package: MOEPatchMerge
// Enums: 1

// Object: Enum MOEPatchMerge.EMoeHDiffPatchErrorCode
enum class EMoeHDiffPatchErrorCode : uint16_t
{
	Success = 0,
	FileAlreadyExistsWithCorrectMD5 = 1,
	FileAlreadyExistsWithWrongMD5 = 2,
	OldFileNotFound = 3,
	DiffFileNotFound = 4,
	InvalidFilePath = 5,
	MergeFailed = 100,
	MergedFileMD5Mismatch = 101,
	MergedFileNotCreated = 102,
	Unknown = 255,
	EMoeHDiffPatchErrorCode_MAX = 256
};

// Package: ProceduralMeshComponent
// Enums: 1

// Object: Enum ProceduralMeshComponent.EProcMeshSliceCapOption
enum class EProcMeshSliceCapOption : uint8_t
{
	NoCap = 0,
	CreateNewSectionForCap = 1,
	UseLastSectionForCap = 2,
	EProcMeshSliceCapOption_MAX = 3
};

// Package: TimeOfDay
// Enums: 3

// Object: Enum TimeOfDay.ECurveTimeType
enum class ECurveTimeType : uint8_t
{
	SunZ = 0,
	TimeOfDay = 1,
	InternalTimeOfDay = 2,
	ECurveTimeType_MAX = 3
};

// Object: Enum TimeOfDay.ESkyMode
enum class ESkyMode : uint8_t
{
	E2DDynamicClouds = 0,
	E2DCloudsUsingColorCurve = 1,
	EVolumetricClouds = 2,
	ESkyMode_MAX = 3
};

// Object: Enum TimeOfDay.E__UDS_SkyMode__pf
enum class E__UDS_SkyMode__pf : uint8_t
{
	EVolumetricClouds = 0,
	EStaticClouds = 1,
	E2DDynamicClouds = 2,
	ENoClouds = 3,
	E2DCloudsUsingColorCurve = 4,
	EVolumetricAurora = 5,
	ENoCloudsUsingColorCurve = 6,
	E__UDS_SkyMode__MAX = 7
};

// Package: XWindSystem
// Enums: 3

// Object: Enum XWindSystem.EWindForceLevel
enum class EWindForceLevel : uint8_t
{
	Custom = 0,
	Still = 1,
	Calm = 2,
	Breezy = 3,
	Strong = 4,
	Violent = 5,
	EWindForceLevel_MAX = 6
};

// Object: Enum XWindSystem.EFoliageRotationMaskMode
enum class EFoliageRotationMaskMode : uint8_t
{
	RotationMaskNegative = 0,
	RotationMaskNull = 1,
	RotationMaskXZ = 2,
	RotationMaskXY = 3,
	RotationMaskYZ = 4,
	RotationMaskX = 5,
	RotationMaskY = 6,
	RotationMaskZ = 7,
	RotationMaskXYZ = 8,
	EFoliageRotationMaskMode_MAX = 9
};

// Object: Enum XWindSystem.ELocalWindShape
enum class ELocalWindShape : uint8_t
{
	XWindLocalType_Sphere = 0,
	XWindLocalType_Cylinder = 1,
	XWindLocalType_Cube = 2,
	XWindLocalType_SphereByVelocity = 3,
	XWindLocalType_MAX = 4
};

// Package: IKUtil
// Enums: 4

// Object: Enum IKUtil.EIKUtil_BlendBySlotType_SecondLayer
enum class EIKUtil_BlendBySlotType_SecondLayer : uint8_t
{
	E_NonSlot = 0,
	E_ClavicleL_Second = 1,
	E_ClavicleR_Second = 2,
	E_Count = 3,
	E_MAX = 4
};

// Object: Enum IKUtil.EIKUtil_BlendBySlotType
enum class EIKUtil_BlendBySlotType : uint8_t
{
	E_NonSlot = 0,
	E_FullBody = 1,
	E_Spine_Absolute = 2,
	E_Spine1_Absolute = 3,
	E_Spine2_Absolute = 4,
	E_ClavicleL_Absolute = 5,
	E_ClavicleR_Absolute = 6,
	E_ClavicleLR_Absolute = 7,
	E_Head_Absolute = 8,
	E_ArmL_Absolute = 9,
	E_ArmR_Absolute = 10,
	E_Additive = 11,
	E_Spine_Blend_Locomotion = 12,
	E_Spine1_Blend_Locomotion = 13,
	E_Spine2_Blend_Locomotion = 14,
	E_ClavicleL_Blend_Locomotion = 15,
	E_ClavicleR_Blend_Locomotion = 16,
	E_ClavicleLR_Blend_Locomotion = 17,
	E_Head_Blend_Locomotion = 18,
	E_ArmL_Blend_Locomotion = 19,
	E_ArmR_Blend_Locomotion = 20,
	E_Climb = 21,
	E_Bone001_Absolute = 22,
	E_Bone001_Blend_Locomotion = 23,
	E_Count = 24,
	E_MAX = 25
};

// Object: Enum IKUtil.EIKUtil_PlayMontageSlot
enum class EIKUtil_PlayMontageSlot : uint8_t
{
	E_FullBody = 0,
	E_Spine = 1,
	E_Spine1 = 2,
	E_Spine2 = 3,
	E_ClavicleL = 4,
	E_ClavicleR = 5,
	E_ClavicleLR = 6,
	E_Head = 7,
	E_ArmL = 8,
	E_ArmR = 9,
	E_Additive = 10,
	E_ClavicleL_Second = 11,
	E_ClavicleR_Second = 12,
	E_Climb = 13,
	E_Bone001 = 14,
	E_Count = 15,
	E_MAX = 16
};

// Object: Enum IKUtil.EIKUtil_TransitionLogic
enum class EIKUtil_TransitionLogic : uint8_t
{
	StandardBlend = 0,
	Inertialization = 1,
	EIKUtil_MAX = 2
};

// Package: AdaptivePerf
// Enums: 4

// Object: Enum AdaptivePerf.EPerformQualityLevels
enum class EPerformQualityLevels : uint8_t
{
	Low = 0,
	Medium = 1,
	High = 2,
	Epic = 3,
	Cinematic = 4,
	Num = 5,
	EPerformQualityLevels_MAX = 6
};

// Object: Enum AdaptivePerf.EPerfModuleType
enum class EPerfModuleType : uint8_t
{
	EPM_Profiler = 0,
	EPM_MAX = 1
};

// Object: Enum AdaptivePerf.EQualityRenderBits
enum class EQualityRenderBits : uint8_t
{
	EQC_Resolution = 0,
	EQC_ViewDistance = 1,
	EQC_GI = 2,
	EQC_AntiAliasing = 3,
	EQC_PostProcess = 4,
	EQC_Foliage = 5,
	EQC_Shadow = 6,
	EQC_Reflection = 7,
	EQC_Texture = 8,
	EQC_Effects = 9,
	EQC_Shading = 10,
	EQC_MAX = 11
};

// Object: Enum AdaptivePerf.EPerformJudgement
enum class EPerformJudgement : uint8_t
{
	EPJ_KEEP = 0,
	EPJ_CPU_BOUND = 1,
	EPJ_GPU_BOUND = 2,
	EPJ_CPU_GPU_BOUND = 3,
	EPJ_UPGRADE = 4,
	EPJ_DOWNGRADE = 5,
	EPJ_CPU_HIGH_LOADING = 6,
	EPJ_GPU_HIGH_LOADING = 7,
	EPJ_THERMAL_NORMAL = 8,
	EPJ_THERMAL_LIGHT = 9,
	EPJ_THERMAL_LIGHT_POWER_HIGH = 10,
	EPJ_THERMAL_MODERATE = 11,
	EPJ_THERMAL_MODERATE_POWER_HIGH = 12,
	EPJ_THERMAL_SEVERE = 13,
	EPJ_THERMAL_SEVERE_POWER_HIGH = 14,
	EPJ_THERMAL_CRITICAL = 15,
	EPJ_MAX = 16
};

// Package: Feature_ChestPVE
// Enums: 19

// Object: Enum Feature_ChestPVE.EChestPVEHitDirection
enum class EChestPVEHitDirection : uint8_t
{
	BackwardHit = 0,
	ForwardHit = 1,
	LeftHit = 2,
	RightHit = 3,
	EChestPVEHitDirection_MAX = 4
};

// Object: Enum Feature_ChestPVE.EBagCellType
enum class EBagCellType : uint8_t
{
	BAGCELL_None = 0,
	BAGCELL_NoVisible = 1,
	BAGCELL_MAX = 2
};

// Object: Enum Feature_ChestPVE.ERefreshBagType_Server
enum class ERefreshBagType_Server : uint8_t
{
	AddItem = 0,
	RemoveItem = 1,
	ItemAttributeChange = 2,
	CellAttributeChange = 3,
	ChangeItem = 4,
	ChangeBagNum = 5,
	ERefreshBagType_Server_MAX = 6
};

// Object: Enum Feature_ChestPVE.FChestPVEBagItemDynicAttributeType
enum class EFChestPVEBagItemDynicAttributeType : uint8_t
{
	Durability = 0,
	FChestPVEBagItemDynicAttributeType_MAX = 1
};

// Object: Enum Feature_ChestPVE.EChestPVECameraDataProcessorPriority
enum class EChestPVECameraDataProcessorPriority : uint8_t
{
	Follow3P_Processor = 1,
	Follow3P_MAX = 2
};

// Object: Enum Feature_ChestPVE.EChestPVECameraModifierLerpType
enum class EChestPVECameraModifierLerpType : uint8_t
{
	Immediate = 0,
	Lerp = 1,
	EChestPVECameraModifierLerpType_MAX = 2
};

// Object: Enum Feature_ChestPVE.EChestPVECameraMainPlayerStatePriority
enum class EChestPVECameraMainPlayerStatePriority : uint8_t
{
	Default = 0,
	InGame = 1,
	EChestPVECameraMainPlayerStatePriority_MAX = 2
};

// Object: Enum Feature_ChestPVE.EChestPVEGASDebugType
enum class EChestPVEGASDebugType : uint16_t
{
	None = 0,
	Abilities = 1,
	Effects = 2,
	Attributes = 4,
	Tags = 8,
	Cooldowns = 16,
	BlockedAbilities = 32,
	All = 255,
	EChestPVEGASDebugType_MAX = 256
};

// Object: Enum Feature_ChestPVE.ETaskExecutionState
enum class ETaskExecutionState : uint8_t
{
	NotStarted = 0,
	Active = 1,
	Completed = 2,
	Interrupted = 3,
	ETaskExecutionState_MAX = 4
};

// Object: Enum Feature_ChestPVE.EChestPVEDeathState
enum class EChestPVEDeathState : uint8_t
{
	NotDead = 0,
	DeathStarted = 1,
	DeathFinished = 2,
	ReviveStarted = 3,
	ReviveFinished = 4,
	EChestPVEDeathState_MAX = 5
};

// Object: Enum Feature_ChestPVE.EChestPVEMonsterMeshSwitch
enum class EChestPVEMonsterMeshSwitch : uint8_t
{
	SkeletalMesh = 1,
	StaticMesh = 2,
	EChestPVEMonsterMeshSwitch_MAX = 3
};

// Object: Enum Feature_ChestPVE.EChestPVESpawnEventType
enum class EChestPVESpawnEventType : uint8_t
{
	GlobalEvent = 1,
	LocalEvent = 2,
	EChestPVESpawnEventType_MAX = 3
};

// Object: Enum Feature_ChestPVE.EChestPVESpawnType
enum class EChestPVESpawnType : uint8_t
{
	Monster = 1,
	Chest = 2,
	Teleport = 3,
	Props = 4,
	TriggerRelationship = 5,
	Test1 = 6,
	Test2 = 7,
	EChestPVESpawnType_MAX = 8
};

// Object: Enum Feature_ChestPVE.EDungeonBranchCapType
enum class EDungeonBranchCapType : uint8_t
{
	InsteadOf = 0,
	AsWellAs = 1,
	EDungeonBranchCapType_MAX = 2
};

// Object: Enum Feature_ChestPVE.EDungeonBranchMode
enum class EDungeonBranchMode : uint8_t
{
	Local = 0,
	Global = 1,
	EDungeonBranchMode_MAX = 2
};

// Object: Enum Feature_ChestPVE.EDungeonGenerateOperator
enum class EDungeonGenerateOperator : uint8_t
{
	None = 0,
	Generate = 1,
	Clear = 2,
	EDungeonGenerateOperator_MAX = 3
};

// Object: Enum Feature_ChestPVE.EDungeonTilePlacementResult
enum class EDungeonTilePlacementResult : uint8_t
{
	Success = 0,
	NoValidTile = 1,
	TemplateIsNull = 2,
	TileIsColliding = 3,
	NewTileIsNull = 4,
	OutOfBounds = 5,
	BoundsIsNull = 6,
	EDungeonTilePlacementResult_MAX = 7
};

// Object: Enum Feature_ChestPVE.EDungeonProcessStatus
enum class EDungeonProcessStatus : uint8_t
{
	NotStart = 0,
	Ready = 1,
	GenerateTile = 2,
	SpawnConnect = 3,
	Success = 4,
	Fail = 5,
	GenerateTemplateTile = 6,
	RunAlo = 7,
	SpawnBlock = 8,
	StartLoading = 20,
	LoadTiles = 21,
	LoadBlockers = 22,
	LoadConnectors = 23,
	EndLoading = 24,
	EDungeonProcessStatus_MAX = 25
};

// Object: Enum Feature_ChestPVE.EDungeonGenerationStatus
enum class EDungeonGenerationStatus : uint8_t
{
	NotStarted = 0,
	PreProcessing = 1,
	MainPath = 2,
	Branching = 3,
	TileInjection = 4,
	PostProcessing = 5,
	Complete = 6,
	Failed = 7,
	EDungeonGenerationStatus_MAX = 8
};

// Package: MoeGameplayAbilitiesCore
// Enums: 23

// Object: Enum MoeGameplayAbilitiesCore.EOGCAbilityInputType
enum class EOGCAbilityInputType : uint8_t
{
	EIT_None = 0,
	EIT_OnPressed = 1,
	EIT_OnRelease = 2,
	EIT_Max = 255
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeTagTriggerType
enum class EMoeTagTriggerType : uint8_t
{
	ET_TagAdded = 1,
	ET_TagRemoved = 2,
	ET_TagEvent = 3,
	ET_MAX = 4
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeIndicatorControlType
enum class EMoeIndicatorControlType : uint8_t
{
	ICT_Normal = 0,
	ICT_SpecialConfig = 1,
	ICT_MAX = 2
};

// Object: Enum MoeGameplayAbilitiesCore.EAbilityParabolaTraceType
enum class EAbilityParabolaTraceType : uint8_t
{
	APT_Channel = 0,
	APT_ObjectTypes = 1,
	APT_Profile = 2,
	APT_MAX = 3
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeDefaultIndicatorDirection
enum class EMoeDefaultIndicatorDirection : uint8_t
{
	CameraForward = 0,
	CameraBack = 1,
	OwnerForward = 2,
	OwnerBack = 3,
	NearestEnemy = 4,
	SmartLock = 5,
	EMoeDefaultIndicatorDirection_MAX = 6
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeAbilityIndicatorCameraType
enum class EMoeAbilityIndicatorCameraType : uint8_t
{
	Pitch = 0,
	Yaw = 1,
	Both = 2,
	EMoeAbilityIndicatorCameraType_MAX = 3
};

// Object: Enum MoeGameplayAbilitiesCore.EAbilityIndicatorType
enum class EAbilityIndicatorType : uint8_t
{
	Direction = 0,
	Position = 1,
	Actor = 2,
	EAbilityIndicatorType_MAX = 3
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeAbilitySelectPlayerType
enum class EMoeAbilitySelectPlayerType : uint8_t
{
	Teammate = 0,
	Enemy = 1,
	Both = 2,
	EMoeAbilitySelectPlayerType_MAX = 3
};

// Object: Enum MoeGameplayAbilitiesCore.EAbilityIgnoreMoveInputType
enum class EAbilityIgnoreMoveInputType : uint8_t
{
	SpecialValue = 0,
	ToActor = 1,
	EAbilityIgnoreMoveInputType_MAX = 2
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeEffectTargetType
enum class EMoeEffectTargetType : uint8_t
{
	Both = 0,
	Teammate = 1,
	Enemy = 2,
	EMoeEffectTargetType_MAX = 3
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeAbleTaskResult
enum class EMoeAbleTaskResult : uint8_t
{
	Invalid = 0,
	Default = 1,
	Success = 2,
	Failed = 4,
	EMoeAbleTaskResult_MAX = 5
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeAblQueryMethod
enum class EMoeAblQueryMethod : uint8_t
{
	ObjectType = 0,
	Channel = 1,
	EMoeAblQueryMethod_MAX = 2
};

// Object: Enum MoeGameplayAbilitiesCore.EAbleBlackboardKeyType
enum class EAbleBlackboardKeyType : uint8_t
{
	ABT_Bool = 0,
	ABT_Int32 = 1,
	ABT_Float = 2,
	ABT_UObject = 3,
	ABT_Vector = 4,
	ABT_String = 5,
	ABT_Rotator = 6,
	ABT_All = 254,
	Max = 255
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeAblComparisonOperator
enum class EMoeAblComparisonOperator : uint8_t
{
	ECO_Greater = 0,
	ECO_Equal = 1,
	ECO_Less = 2,
	ECO_LessOrEqual = 3,
	ECO_GreaterOrEqual = 4,
	ECO_MAX = 5
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeAbleTargetSource
enum class EMoeAbleTargetSource : uint8_t
{
	Self = 0,
	Owner = 1,
	Instigator = 2,
	CustomLocation = 3,
	EMoeAbleTargetSource_MAX = 4
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeAblTargetingFilterSort
enum class EMoeAblTargetingFilterSort : uint8_t
{
	MoeAblTargetFilterSort_Ascending = 0,
	MoeAblTargetFilterSort_Descending = 1,
	MoeAblTargetFilterSort_MAX = 2
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeParamOperation
enum class EMoeParamOperation : uint8_t
{
	Additive = 0,
	Subtractive = 1,
	Multiplicitive = 2,
	Division = 3,
	Max = 255
};

// Object: Enum MoeGameplayAbilitiesCore.EEquipmentAttachTarget
enum class EEquipmentAttachTarget : uint8_t
{
	RootComponent = 0,
	MeshComponent = 1,
	TagComponent = 2,
	EEquipmentAttachTarget_MAX = 3
};

// Object: Enum MoeGameplayAbilitiesCore.EOGCAbilityCoolDownApplyTime
enum class EOGCAbilityCoolDownApplyTime : uint8_t
{
	ECA_AbilityActivate = 0,
	ECA_AbilityEnd = 1,
	ECA_AbilityDuration = 2,
	ECA_Max = 255
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeBufferedTouchStartPolicy
enum class EMoeBufferedTouchStartPolicy : uint8_t
{
	OnTouchEvent = 0,
	OnAbilityReady = 1,
	EMoeBufferedTouchStartPolicy_MAX = 2
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeBuffStackStrategyType
enum class EMoeBuffStackStrategyType : uint8_t
{
	OnlyDuration = 0,
	StackAll = 1,
	EMoeBuffStackStrategyType_MAX = 2
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeBuffStackType
enum class EMoeBuffStackType : uint8_t
{
	NoStack = 0,
	StackByTarget = 1,
	EMoeBuffStackType_MAX = 2
};

// Object: Enum MoeGameplayAbilitiesCore.EMoeVariantType
enum class EMoeVariantType : uint8_t
{
	NONE = 0,
	VT_FLOAT = 1,
	VT_BOOL = 2,
	VT_INT32 = 3,
	VT_Vector = 4,
	EMoeVariantType_MAX = 5
};

// Package: MoeGameplayAbilities
// Enums: 48

// Object: Enum MoeGameplayAbilities.EOGCIndicateState
enum class EOGCIndicateState : uint8_t
{
	None = 0,
	TouchStart = 1,
	Touching = 2,
	TouchEnd = 3,
	EOGCIndicateState_MAX = 4
};

// Object: Enum MoeGameplayAbilities.EMoeAblAddForceType
enum class EMoeAblAddForceType : uint8_t
{
	EFT_IndicatorDirection = 0,
	EFT_ToSpecialPoint = 1,
	EFT_ToSpecialActor = 2,
	EFT_Custom = 3,
	EFT_HorizontalTangent = 4,
	EFT_VerticalTangent = 5,
	EFT_Velocity = 6,
	EFT_CustomDir = 7,
	EFT_MAX = 8
};

// Object: Enum MoeGameplayAbilities.EMoeAblCollisionDetectType
enum class EMoeAblCollisionDetectType : uint8_t
{
	ECT_ObjectType = 0,
	ECT_Channel = 1,
	ECT_Profile = 2,
	ECT_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EAttackShapeType
enum class EAttackShapeType : uint8_t
{
	Box = 0,
	Sphere = 1,
	Capsule = 2,
	EAttackShapeType_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAblAttributeType
enum class EMoeAblAttributeType : uint8_t
{
	FloatType = 0,
	BoolType = 1,
	StringType = 2,
	EMoeAblAttributeType_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAblParticlePlayCondition
enum class EMoeAblParticlePlayCondition : uint8_t
{
	Both = 1,
	OnlySelf = 2,
	Teammate = 3,
	Enemy = 4,
	SelfAndTeammate = 5,
	ClientWithSpecifiedTag = 6,
	ClientWithSpecifiedGameplayTag = 7,
	EMoeAblParticlePlayCondition_MAX = 8
};

// Object: Enum MoeGameplayAbilities.ESpawnActorIntervalType
enum class ESpawnActorIntervalType : uint8_t
{
	TimeInterval = 0,
	DistanceInterval = 1,
	ESpawnActorIntervalType_MAX = 2
};

// Object: Enum MoeGameplayAbilities.ESpawnActorAnchoringPosition
enum class ESpawnActorAnchoringPosition : uint8_t
{
	ActorLocation = 0,
	FootLocation = 1,
	ESpawnActorAnchoringPosition_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeAblChangeMeshType
enum class EMoeAblChangeMeshType : uint8_t
{
	StaticMesh = 0,
	SkeletalMesh = 1,
	EMoeAblChangeMeshType_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeAttachCompType
enum class EMoeAttachCompType : uint8_t
{
	Skeletal = 0,
	Root = 1,
	Custom = 2,
	EMoeAttachCompType_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAblOffsetSpaceType
enum class EMoeAblOffsetSpaceType : uint8_t
{
	World = 0,
	Local = 1,
	AimOffset = 2,
	EMoeAblOffsetSpaceType_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAblActorOffsetSpaceType
enum class EMoeAblActorOffsetSpaceType : uint8_t
{
	World = 0,
	Local = 1,
	EMoeAblActorOffsetSpaceType_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeAblShapeType
enum class EMoeAblShapeType : uint8_t
{
	Line = 0,
	Sphere = 1,
	Box = 2,
	EMoeAblShapeType_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EAblTaskTarget
enum class EAblTaskTarget : uint8_t
{
	Self = 0,
	Target = 1,
	Max = 255
};

// Object: Enum MoeGameplayAbilities.EMoeAblAbilityProjectileMoveType
enum class EMoeAblAbilityProjectileMoveType : uint8_t
{
	StraightLine = 0,
	Parabola = 1,
	EMoeAblAbilityProjectileMoveType_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeAblActorBlackboardType
enum class EMoeAblActorBlackboardType : uint8_t
{
	EABT_ActorLocation = 0,
	EABT_ActorXDirection = 1,
	EABT_ActorYDirection = 2,
	EABT_ActorVelocity = 3,
	EABT_ActorDirToActor = 5,
	EABT_ActorDirToPoint = 6,
	EABT_HorizontalTangent = 7,
	EABT_MAX = 8
};

// Object: Enum MoeGameplayAbilities.EMoeCollisionIgnoreMask
enum class EMoeCollisionIgnoreMask : uint8_t
{
	None = 0,
	Pawn = 1,
	GroundOrWall = 2,
	EMoeCollisionIgnoreMask_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAblCharacterMoveDirType
enum class EMoeAblCharacterMoveDirType : uint8_t
{
	EASD_OwnerForward = 1,
	EAD_WorldZDir = 2,
	EAD_BlackboardDir = 3,
	EAD_CustomParam = 4,
	EMoeAblCharacterMoveDirType_MAX = 5
};

// Object: Enum MoeGameplayAbilities.EMoeAblCharacterMoveSpeedType
enum class EMoeAblCharacterMoveSpeedType : uint8_t
{
	EAS_OwnerSpeed = 1,
	EAS_SpecialSpeed = 2,
	EAS_SpeedCurve = 3,
	EAS_MAX = 4
};

// Object: Enum MoeGameplayAbilities.EMoeAblCharacterMoveType
enum class EMoeAblCharacterMoveType : uint8_t
{
	EAM_LineMove = 1,
	EAM_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeAblCharacterMoveMethod
enum class EMoeAblCharacterMoveMethod : uint8_t
{
	AddImpulse = 0,
	AddOffset = 1,
	EMoeAblCharacterMoveMethod_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeAblCharAddOffsetDirection
enum class EMoeAblCharAddOffsetDirection : uint8_t
{
	Forward = 0,
	Up = 1,
	Special = 2,
	EMoeAblCharAddOffsetDirection_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAblFloatParam_CooldownTargetType
enum class EMoeAblFloatParam_CooldownTargetType : uint8_t
{
	Self = 0,
	BaseTag = 1,
	BaseGAClass = 2,
	EMoeAblFloatParam_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAblHitParticlePlayCondition
enum class EMoeAblHitParticlePlayCondition : uint8_t
{
	Both = 1,
	OnlySelf = 2,
	EMoeAblHitParticlePlayCondition_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAblKeepCharacterRotationType
enum class EMoeAblKeepCharacterRotationType : uint8_t
{
	FaceToSpecialPoint = 0,
	FaceToSpecialActor = 1,
	MAX = 255
};

// Object: Enum MoeGameplayAbilities.EMoeAblMoveTargetType
enum class EMoeAblMoveTargetType : uint8_t
{
	MoveToSpecialPoint = 0,
	MoveToSpecialActor = 1,
	Max = 255
};

// Object: Enum MoeGameplayAbilities.EMoeKeepDistanceType
enum class EMoeKeepDistanceType : uint8_t
{
	SpringForce = 0,
	AddOffset = 1,
	EMoeKeepDistanceType_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeMoveByCurveSpace
enum class EMoeMoveByCurveSpace : uint8_t
{
	World = 0,
	Local = 1,
	ActorForward = 2,
	EMoeMoveByCurveSpace_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAblShowWidgetType
enum class EMoeAblShowWidgetType : uint8_t
{
	EST_OnlyTarget = 1,
	EST_TargetAndSelf = 2,
	EST_Both = 3,
	EST_MAX = 4
};

// Object: Enum MoeGameplayAbilities.EMoeSoundPlayType
enum class EMoeSoundPlayType : uint8_t
{
	EST_Both = 0,
	EST_OnlyAutonomousProxy = 1,
	EST_OnlySimulatedProxy = 2,
	EST_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeRemoveBuffType
enum class EMoeRemoveBuffType : uint8_t
{
	ERT_All = 0,
	ERT_SpeicalSource = 1,
	ERT_ExcludeSource = 2,
	ERT_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EOGCTeleportType
enum class EOGCTeleportType : uint8_t
{
	EOT_Offset = 0,
	EOT_Indicator = 1,
	EOT_VectorKey = 2,
	EOT_ActorKey = 3,
	EOT_Max = 255
};

// Object: Enum MoeGameplayAbilities.EMoeAblSetFloatType
enum class EMoeAblSetFloatType : uint8_t
{
	EABT_ActorDistance = 0,
	EABT_VectorAngle = 1,
	EABT_ActorYaw = 2,
	EABT_InverseActorYaw = 3,
	EABT_MAX = 4
};

// Object: Enum MoeGameplayAbilities.EMoeAblAbilityAutoDestroyType
enum class EMoeAblAbilityAutoDestroyType : uint8_t
{
	DestroyOnTaskEnd = 0,
	SpecialLifeSpan = 1,
	EMoeAblAbilityAutoDestroyType_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeAblSpawnAbilityGearRotationType
enum class EMoeAblSpawnAbilityGearRotationType : uint8_t
{
	OwnerActorRotation = 0,
	FaceOwner = 1,
	CustomRotator = 2,
	CustomDirection = 3,
	EMoeAblSpawnAbilityGearRotationType_MAX = 4
};

// Object: Enum MoeGameplayAbilities.EMoeAblSpawnAbilityGearPositionType
enum class EMoeAblSpawnAbilityGearPositionType : uint8_t
{
	OwnerActorLocation = 0,
	BlackboardKey = 1,
	EMoeAblSpawnAbilityGearPositionType_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeAblAbilityCDAdjustTargetType
enum class EMoeAblAbilityCDAdjustTargetType : uint8_t
{
	Self = 0,
	BaseTag = 1,
	BaseGAClass = 2,
	EMoeAblAbilityCDAdjustTargetType_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAblAbilityCDAdjustType
enum class EMoeAblAbilityCDAdjustType : uint8_t
{
	Override = 0,
	AddValue = 1,
	MultiScale = 2,
	EMoeAblAbilityCDAdjustType_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoeAttachToComponentType
enum class EMoeAttachToComponentType : uint8_t
{
	Capsule = 0,
	Mesh = 1,
	EMoeAttachToComponentType_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeAttachLerpRotationType
enum class EMoeAttachLerpRotationType : uint8_t
{
	Actor = 0,
	Controller = 1,
	Mesh = 2,
	EMoeAttachLerpRotationType_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EDetectionFaction
enum class EDetectionFaction : uint8_t
{
	All = 0,
	Enemy = 1,
	Friend = 2,
	EDetectionFaction_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EDetectionShape
enum class EDetectionShape : uint8_t
{
	Box = 0,
	Sphere = 1,
	EDetectionShape_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeCrosshairIndicatorStage
enum class EMoeCrosshairIndicatorStage : uint8_t
{
	Default = 0,
	PreSelected = 1,
	Selecting = 2,
	Locking = 3,
	Locked = 4,
	Max = 5
};

// Object: Enum MoeGameplayAbilities.EProjectileMoveDirection
enum class EProjectileMoveDirection : uint8_t
{
	ForwardInLocalSpace = 0,
	SpecialDirection = 1,
	AbilityOwnerSpace = 2,
	EProjectileMoveDirection_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EProjectileMoveMode
enum class EProjectileMoveMode : uint8_t
{
	Line = 0,
	EProjectileMoveMode_MAX = 1
};

// Object: Enum MoeGameplayAbilities.EProjectileOffsetSpace
enum class EProjectileOffsetSpace : uint8_t
{
	World = 0,
	Local = 1,
	Projectile = 2,
	EProjectileOffsetSpace_MAX = 3
};

// Object: Enum MoeGameplayAbilities.EMoveSpace
enum class EMoveSpace : uint8_t
{
	World = 0,
	Local = 1,
	EMoveSpace_MAX = 2
};

// Object: Enum MoeGameplayAbilities.EMoeInputMode
enum class EMoeInputMode : uint8_t
{
	World = 0,
	Camera = 1,
	Camera3D = 2,
	Pawn = 3,
	EMoeInputMode_MAX = 4
};

// Package: Feature_Chest
// Enums: 13

// Object: Enum Feature_Chest.EMoeAnimModule_ChestBossActionState
enum class EMoeAnimModule_ChestBossActionState : uint8_t
{
	Moe_Default = 0,
	Moe_BossCatch = 1,
	Moe_BossRelease = 2,
	Moe_BossExile = 3,
	Moe_BossStub = 4,
	Moe_BossClimb = 5,
	Moe_BossBreakBoard = 6,
	Moe_BossCommonAction = 7,
	Moe_BossSearchHidingOut = 8,
	Moe_MAX = 9
};

// Object: Enum Feature_Chest.EMoeAnimModule_ChestBossUpperBodyActionState
enum class EMoeAnimModule_ChestBossUpperBodyActionState : uint8_t
{
	Moe_Default = 0,
	Moe_BossAttack1Before = 1,
	Moe_BossCarrying = 2,
	Moe_CommonUpper = 3,
	Moe_MAX = 4
};

// Object: Enum Feature_Chest.EChestCameraDataProcessorPriority
enum class EChestCameraDataProcessorPriority : uint8_t
{
	Modifier_Processor = 1,
	YawClamp_Processor = 2,
	EChestCameraDataProcessorPriority_MAX = 3
};

// Object: Enum Feature_Chest.EChestCameraModifierLerpType
enum class EChestCameraModifierLerpType : uint8_t
{
	Immediate = 0,
	Lerp = 1,
	EChestCameraModifierLerpType_MAX = 2
};

// Object: Enum Feature_Chest.EChestCameraMainPlayerStatePriority
enum class EChestCameraMainPlayerStatePriority : uint8_t
{
	Default = 0,
	Character = 1,
	Boss = 2,
	EChestCameraMainPlayerStatePriority_MAX = 3
};

// Object: Enum Feature_Chest.EMoeAnimModule_ChestBaseLocomotionState
enum class EMoeAnimModule_ChestBaseLocomotionState : uint8_t
{
	Chest_Default = 0,
	Chest_Injuried = 1,
	Chest_Dying = 2,
	Chest_CaughtInBoss = 3,
	Chest_CaughtInChair = 4,
	Chest_CaughtInGourd = 5,
	Chest_Walking = 6,
	Chest_MAX = 7
};

// Object: Enum Feature_Chest.EMoeAnimModule_ChestUpperBodyActionState
enum class EMoeAnimModule_ChestUpperBodyActionState : uint8_t
{
	Moe_Default = 0,
	Moe_BossAttack1Before = 1,
	Moe_CommonSkillLoop = 2,
	Moe_RescuedSuccess = 3,
	Moe_RescueTeammates = 4,
	Moe_SlowClimbWindow = 5,
	Moe_FastClimbWindow = 6,
	Moe_SlowClimbBlock = 7,
	Moe_FastClimbBlock = 8,
	Moe_CaughtStruggle = 9,
	Moe_BeHit = 10,
	Moe_Idle2LastLand = 11,
	Moe_LastLand2Idle = 12,
	Moe_ReadingStarChart = 13,
	Moe_RescueFromAltar = 14,
	Moe_DecipherGate = 15,
	Moe_PlayerDeadForDying = 16,
	Moe_PlayerPutBoard = 17,
	Moe_SomersaultMove = 18,
	Moe_GrabHit = 19,
	Moe_BeGrabHit = 20,
	Chest_HealTeammates = 21,
	Chest_CommonInteract = 22,
	EMoeAnimModule_MAX = 23
};

// Object: Enum Feature_Chest.EChestCustomMovementMode
enum class EChestCustomMovementMode : uint8_t
{
	None = 0,
	DraculaBatFly = 1,
	EChestCustomMovementMode_MAX = 2
};

// Object: Enum Feature_Chest.EChestSlideAlongSurfaceType
enum class EChestSlideAlongSurfaceType : uint8_t
{
	Default = 0,
	ReduceForward = 1,
	Redirect = 2,
	EChestSlideAlongSurfaceType_MAX = 3
};

// Object: Enum Feature_Chest.EChestPreparePhase
enum class EChestPreparePhase : uint8_t
{
	GPP_WaitForGenerateProp = 0,
	GPP_GenerateProp = 1,
	GPP_PropFly = 2,
	GPP_PropStopFly = 3,
	GPP_PropGenerated = 4,
	GPP_MAX = 5
};

// Object: Enum Feature_Chest.EChestGamePhase
enum class EChestGamePhase : uint8_t
{
	GP_Idle = 0,
	GP_Prepare = 1,
	GP_Playing = 2,
	GP_Finish = 3,
	GP_End = 4,
	GP_MAX = 5
};

// Object: Enum Feature_Chest.EChestInteractiveActorType
enum class EChestInteractiveActorType : uint8_t
{
	None = 0,
	StarChart = 1,
	Altar = 2,
	Password = 3,
	Board = 4,
	Window = 5,
	TrumpAltar = 6,
	Gachapon = 7,
	Fountain = 8,
	Vehicle = 9,
	StarTeleport = 10,
	CubeTrapRescue = 11,
	CubeTrapDestroy = 12,
	BloodTrapDestroy = 13,
	Cellar = 14,
	Chest = 15,
	Hideout = 16,
	ChestDoor = 17,
	SafeArea = 18,
	Reserved1 = 19,
	Reserved2 = 20,
	Reserved3 = 21,
	Reserved4 = 22,
	Reserved5 = 23,
	EChestInteractiveActorType_MAX = 24
};

// Object: Enum Feature_Chest.EChestCameraType
enum class EChestCameraType : uint8_t
{
	Default = 0,
	Character = 1,
	Boss = 2,
	EChestCameraType_MAX = 3
};

// Package: MainGame
// Enums: 16

// Object: Enum MainGame.EBnbBombAction
enum class EBnbBombAction : uint8_t
{
	Create = 0,
	AirDrop = 1,
	Move = 2,
	Bomb = 3,
	Sink = 4,
	EBnbBombAction_MAX = 5
};

// Object: Enum MainGame.EBnbDirection
enum class EBnbDirection : uint8_t
{
	Front = 0,
	Back = 1,
	Left = 2,
	Right = 3,
	Up = 4,
	Down = 5,
	Other = 6,
	EBnbDirection_MAX = 7
};

// Object: Enum MainGame.EBnbMoveState
enum class EBnbMoveState : uint8_t
{
	Sleep = 0,
	WaitMove = 1,
	Moving = 2,
	EBnbMoveState_MAX = 3
};

// Object: Enum MainGame.EAbleSetInputDirectionType
enum class EAbleSetInputDirectionType : uint8_t
{
	ABT_Ignore = 0,
	ABT_Keep = 1,
	ABT_MAX = 2
};

// Object: Enum MainGame.EMoeBnbBlockType
enum class EMoeBnbBlockType : uint8_t
{
	Floor = 0,
	Bomb = 1,
	EMoeBnbBlockType_MAX = 2
};

// Object: Enum MainGame.EMoeBnbBombState
enum class EMoeBnbBombState : uint8_t
{
	None = 0,
	Creating = 1,
	Falling = 2,
	Moving = 3,
	Bombing = 4,
	FloorDestroy = 5,
	EMoeBnbBombState_MAX = 6
};

// Object: Enum MainGame.EMoeBnbDirect
enum class EMoeBnbDirect : uint8_t
{
	Front = 0,
	Back = 1,
	Left = 2,
	Right = 3,
	Up = 4,
	Down = 5,
	Other = 6,
	EMoeBnbDirect_MAX = 7
};

// Object: Enum MainGame.EAdsorbOffsetType
enum class EAdsorbOffsetType : uint8_t
{
	Offset_XY = 0,
	Offset_Z = 1,
	Offset_All = 2,
	Offset_MAX = 3
};

// Object: Enum MainGame.ESpeedType
enum class ESpeedType : uint8_t
{
	ES_WalkSpeed = 0,
	ES_FlySpeed = 1,
	ES_MAX = 2
};

// Object: Enum MainGame.EMstPatrolType
enum class EMstPatrolType : uint8_t
{
	None = 0,
	RandomInCircle = 1,
	FlowSpecialPoints = 2,
	Max = 255
};

// Object: Enum MainGame.EOGCCameraDataProcessorPriority
enum class EOGCCameraDataProcessorPriority : uint8_t
{
	Follow2D_Processor = 1,
	RotateToTarget_Processor = 2,
	EOGCCameraDataProcessorPriority_MAX = 3
};

// Object: Enum MainGame.EOGCCameraMainPlayerStatePriority
enum class EOGCCameraMainPlayerStatePriority : uint8_t
{
	Invalid = 0,
	Follow2D = 1,
	EOGCCameraMainPlayerStatePriority_MAX = 2
};

// Object: Enum MainGame.EMoeGhostCatchBuffID
enum class EMoeGhostCatchBuffID : uint8_t
{
	GHOST = 0,
	HALF_GHOST = 1,
	DEAD = 2,
	EMoeGhostCatchBuffID_MAX = 3
};

// Object: Enum MainGame.EScoreGameAddScoreType
enum class EScoreGameAddScoreType : uint8_t
{
	Both = 0,
	Player = 1,
	Team = 2,
	TeamSide = 3,
	Error = 4,
	EScoreGameAddScoreType_MAX = 5
};

// Object: Enum MainGame.EScoreGameType
enum class EScoreGameType : uint8_t
{
	Personal = 0,
	Regimental = 1,
	Error = 2,
	EScoreGameType_MAX = 3
};

// Object: Enum MainGame.EExtraRoundType
enum class EExtraRoundType : uint8_t
{
	NoExtraRound = 0,
	FixedTime = 1,
	DecidingGame = 2,
	EExtraRoundType_MAX = 3
};

// Package: Community
// Enums: 17

// Object: Enum Community.EMoeRiddenVehicleState
enum class EMoeRiddenVehicleState : uint8_t
{
	None = 0,
	Idle = 1,
	MoveForward = 2,
	TurnLeft = 3,
	TurnRight = 4,
	Jet = 5,
	IdleShow = 6,
	EMoeRiddenVehicleState_MAX = 7
};

// Object: Enum Community.EPetAnimState
enum class EPetAnimState : uint8_t
{
	State_Normal = 0,
	State_Move = 1,
	State_Sleep = 2,
	State_Rest = 3,
	State_Struggle = 4,
	State_MAX = 5
};

// Object: Enum Community.EMoePetMotionState
enum class EMoePetMotionState : uint8_t
{
	Idle = 0,
	FlyIdle = 1,
	FlyExit = 2,
	Lifted = 3,
	Sleeping = 4,
	Ride = 5,
	Max = 64
};

// Object: Enum Community.EMoePetActionState
enum class EMoePetActionState : uint8_t
{
	Idle = 0,
	Max = 64
};

// Object: Enum Community.EMoeAbilityInteractiveLerpType
enum class EMoeAbilityInteractiveLerpType : uint8_t
{
	Special = 0,
	Actor = 1,
	Socket = 2,
	Relative = 3,
	Max = 255
};

// Object: Enum Community.EMoeBubbleGunState
enum class EMoeBubbleGunState : uint8_t
{
	None = 0,
	NoPick = 1,
	OnHand = 2,
	PreShoot = 3,
	Shooting = 4,
	End = 5,
	EMoeBubbleGunState_MAX = 6
};

// Object: Enum Community.ECommunityBubbleState
enum class ECommunityBubbleState : uint8_t
{
	WaitEnter = 0,
	IntoBubble = 1,
	Playing = 2,
	WaitDestroy = 3,
	ECommunityBubbleState_MAX = 4
};

// Object: Enum Community.EFireworkType
enum class EFireworkType : uint8_t
{
	None = 0,
	Normal = 1,
	Pic = 2,
	Text = 3,
	EFireworkType_MAX = 4
};

// Object: Enum Community.EFireworkPartyStage
enum class EFireworkPartyStage : uint8_t
{
	Reading = 0,
	Playing = 1,
	PreClose = 2,
	Close = 3,
	EFireworkPartyStage_MAX = 4
};

// Object: Enum Community.ECharLiftSPPetState
enum class ECharLiftSPPetState : uint8_t
{
	None = 0,
	Lift_Begin = 1,
	Lift = 2,
	Lift_End = 3,
	Grip = 4,
	Throw_Begin = 5,
	Throw = 6,
	Throw_End = 7,
	ECharLiftSPPetState_MAX = 8
};

// Object: Enum Community.ECollectedResult
enum class ECollectedResult : uint8_t
{
	Succeed = 0,
	DropItemInCollectList = 1,
	DropItemAbsent = 2,
	DropItemStateError = 3,
	DropItemSpaceError = 4,
	Unknown = 5,
	ECollectedResult_MAX = 6
};

// Object: Enum Community.EDropItemState
enum class EDropItemState : uint8_t
{
	Falling = 0,
	Landing = 1,
	Collecting = 2,
	Collected = 3,
	Abandoned = 4,
	EDropItemState_MAX = 5
};

// Object: Enum Community.EActivityState
enum class EActivityState : uint8_t
{
	Ready = 0,
	Arriving = 1,
	Running = 2,
	Returning = 3,
	Completed = 4,
	EActivityState_MAX = 5
};

// Object: Enum Community.EMoeCommunityReportPath
enum class EMoeCommunityReportPath : uint8_t
{
	Unknown = 0,
	DSToGS = 1,
	ClientToGS = 2,
	EMoeCommunityReportPath_MAX = 3
};

// Object: Enum Community.ESeesawRunType
enum class ESeesawRunType : uint8_t
{
	Balance = 0,
	LeftDown = 1,
	RightDown = 2,
	DownUpRun = 3,
	ESeesawRunType_MAX = 4
};

// Object: Enum Community.EDoubleDanceExitType
enum class EDoubleDanceExitType : uint8_t
{
	None = 0,
	SelfClickExit = 1,
	OtherClickExit = 2,
	AutoExit = 3,
	EDoubleDanceExitType_MAX = 4
};

// Object: Enum Community.EQteClickType
enum class EQteClickType : uint8_t
{
	Miss = 0,
	Good = 1,
	Perfect = 2,
	EQteClickType_MAX = 3
};

// Package: Feature_SP
// Enums: 374

// Object: Enum Feature_SP.ESPUIButtonType
enum class ESPUIButtonType : uint8_t
{
	None = 0,
	LeftJoyStick = 1,
	Jump = 2,
	Dive = 3,
	CaptureBall = 4,
	SummonPet = 5,
	AttackRight = 6,
	AttackLeft = 7,
	Reload = 8,
	SwitchWeapon = 9,
	SelectWeapon = 10,
	FriendSkill = 11,
	UltimateSkill = 12,
	PetAttack = 13,
	GoBackCity = 14,
	OpenBuild = 15,
	Morph = 16,
	MountSkill01 = 17,
	MountSkill02 = 18,
	ESPUIButtonType_MAX = 19
};

// Object: Enum Feature_SP.ESPSkillSlot
enum class ESPSkillSlot : uint8_t
{
	None = 0,
	Slot_One = 1,
	Slot_Two = 2,
	Slot_Three = 3,
	Slot_Four = 4,
	Slot_Five = 5,
	Slot_Six = 6,
	Slot_Seven = 7,
	Slot_Eight = 8,
	Slot_Nine = 9,
	Slot_Ex10 = 10,
	Slot_Ex11 = 11,
	Slot_Ex12 = 12,
	Slot_Ex13 = 13,
	Slot_Ex14 = 14,
	Slot_Ex15 = 15,
	Slot_Ex16 = 16,
	Slot_Ex17 = 17,
	Slot_Ex18 = 18,
	Slot_Ex19 = 19,
	Slot_Ex20 = 20,
	Slot_Ex21 = 21,
	Slot_Ex22 = 22,
	Slot_Ex23 = 23,
	Slot_Ex24 = 24,
	Slot_Ex25 = 25,
	Slot_Ex26 = 26,
	Slot_Ex27 = 27,
	Slot_Ex28 = 28,
	Slot_Ex29 = 29,
	Slot_Ex30 = 30,
	Special_Start = 50,
	Special_Capture = 51,
	Special_Unleash = 52,
	Special_Leash = 53,
	Special_Roll = 54,
	Special_Friend = 55,
	Special_CancelAiming = 56,
	Special_Stroke = 57,
	Special_Feed = 58,
	Special_Stare = 59,
	Special_UltimateMove = 60,
	Tumbling_Forward = 61,
	Tumbling_Left = 62,
	Tumbling_Back = 63,
	Tumbling_Right = 64,
	Rescue = 65,
	Rebirth = 66,
	StopSprint = 67,
	Dying = 68,
	Special_Relieve = 69,
	Wild_Sleeping = 80,
	Wild_Relax1 = 81,
	Wild_Relax2 = 82,
	Wild_Sober = 83,
	Wild_AirRelax1 = 84,
	Wild_AirRelax2 = 85,
	Wild_Relax3 = 86,
	Wild_Relax4 = 87,
	Wild_AirRelax3 = 88,
	Wild_AirRelax4 = 89,
	LookAround = 90,
	Encounter = 91,
	Special_StepBack = 92,
	Special_Transfigure1_On = 93,
	Special_Transfigure1_Off = 94,
	Special_Friend2 = 95,
	Special_End = 99,
	Soc_Start = 100,
	Soc_Lumbering = 101,
	Soc_Construction = 102,
	Soc_Handicraft = 103,
	Soc_Cooking = 104,
	Soc_Watering = 105,
	Soc_Planting = 106,
	Soc_Harvesting = 107,
	Soc_Splitting = 108,
	Soc_Mining = 109,
	Special_DrilledInto = 110,
	Special_DrilledOut = 111,
	Special_Loop = 112,
	Special_FeedTwo = 113,
	SOC_Cooking_Additional = 114,
	SOC_Cooking_Additional = 115,
	Slot_Max = 127,
	Affinity_ExpMultiply = 140,
	Affinity_Interact = 141,
	Affinity_EnhancedSkill = 142,
	Affinity_Appearance = 143,
	Special_AffinityOne = 144,
	Special_AffinityTwo = 145,
	Special_AffinityThree = 146,
	PVP_Attack = 147,
	PVP_Special = 148,
	PVP_Passive = 149,
	PVP_Battlecry = 150,
	PVP_Active = 151,
	PVP_EnhanceAttack = 152,
	Ability_BattleTalentManage = 180,
	Special_ReviveMaster = 199,
	Special_AICastSkillSlot = 200,
	Special_Assist1 = 201,
	Special_Assist2 = 202,
	Special_Assist3 = 203,
	Special_Assist4 = 204,
	Special_Assist5 = 205,
	Morph_Enter = 225,
	Morph_Exit = 226,
	Morph_ForceExit = 227,
	Morph_Slot1 = 230,
	Morph_Slot2 = 231,
	Morph_Slot3 = 232,
	Morph_Slot4 = 233,
	Morph_Slot5 = 234,
	Morph_Tumbling = 240,
	Morph_Tumbling_Forward = 241,
	Morph_Tumbling_Left = 242,
	Morph_Tumbling_Back = 243,
	Morph_Tumbling_Right = 244,
	ESPSkillSlot_MAX = 245
};

// Object: Enum Feature_SP.ESPMsgFromPlayerToMonster
enum class ESPMsgFromPlayerToMonster : uint8_t
{
	None = 0,
	AskFriendSkillCmd = 1,
	CancelFriendSkillCmd = 2,
	ESPMsgFromPlayerToMonster_MAX = 3
};

// Object: Enum Feature_SP.ESPMsgFromMonsterToPlayer
enum class ESPMsgFromMonsterToPlayer : uint8_t
{
	None = 0,
	FriendSkillInterrupted = 1,
	FriendSkillStartPlay = 2,
	FriendSkillPlaying = 3,
	FriendSkillFinish = 4,
	ESPMsgFromMonsterToPlayer_MAX = 5
};

// Object: Enum Feature_SP.ESPActorType
enum class ESPActorType : uint8_t
{
	None = 0,
	Player = 1,
	Pet = 2,
	Resource_Begin = 3,
	Resource_Tree = 4,
	Resource_Rock = 5,
	Resource_End = 6,
	Building = 7,
	DropItem = 8,
	Summon = 9,
	PackageAirWall = 10,
	Water = 11,
	CoveredTreasure = 12,
	GroupBuilding = 13,
	MonsterWound = 14,
	ShootBalloon = 15,
	ESPActorType_MAX = 16
};

// Object: Enum Feature_SP.ESPRideSysPhaseChangeReason
enum class ESPRideSysPhaseChangeReason : uint8_t
{
	Unknown = 0,
	PlayerInput = 1,
	ServerRequest = 2,
	Cheat = 3,
	BeginPlay = 4,
	StartLogic = 5,
	PawnClientRestart = 6,
	RepNotify = 7,
	Interrupted = 8,
	Dead = 9,
	EndPlay = 10,
	RiddenSysStart = 11,
	EnterMorph = 12,
	ExitMorph = 13,
	MaxReason = 14,
	ESPRideSysPhaseChangeReason_MAX = 15
};

// Object: Enum Feature_SP.ESPRideSysPhase
enum class ESPRideSysPhase : uint8_t
{
	Uninitialized = 0,
	Normal = 1,
	Mount = 2,
	Riding = 3,
	Dismount = 4,
	ESPRideSysPhase_MAX = 5
};

// Object: Enum Feature_SP.ESPRideMovePhase
enum class ESPRideMovePhase : uint8_t
{
	Disabled = 0,
	Land = 1,
	Air = 2,
	Water = 3,
	ESPRideMovePhase_MAX = 4
};

// Object: Enum Feature_SP.ESP_MonsterStateType
enum class ESP_MonsterStateType : uint8_t
{
	Normal = 0,
	Freeze = 1,
	Falling = 2,
	Dead = 3,
	Sleeping = 4,
	Hit = 5,
	Building = 6,
	Handworking = 7,
	Pharmacying = 8,
	Logging = 9,
	Mining = 10,
	Cooking = 11,
	Seeding = 12,
	Pouring = 13,
	Cheer = 14,
	Combat = 15,
	Escape = 16,
	Eat = 17,
	Stroke = 18,
	Lift = 19,
	Swim = 20,
	Carrying = 21,
	Cooling = 22,
	Electric = 23,
	Grazing = 24,
	Soak = 25,
	Resist = 26,
	Confront = 27,
	Drown = 28,
	DrownDead = 29,
	ESP_MAX = 30
};

// Object: Enum Feature_SP.ESPSpawnMonsterType
enum class ESPSpawnMonsterType : uint16_t
{
	SpawnPoint = 0,
	SpawnCombatTeam = 1,
	SpawnTerminal = 2,
	SpawnDungeon = 3,
	SpawnPVP = 4,
	SpawnPreview = 5,
	Invalid = 255,
	ESPSpawnMonsterType_MAX = 256
};

// Object: Enum Feature_SP.ESPMonsterBodyShape
enum class ESPMonsterBodyShape : uint8_t
{
	Normal = 0,
	Huge = 1,
	ESPMonsterBodyShape_MAX = 2
};

// Object: Enum Feature_SP.ESPNavMeshLayer
enum class ESPNavMeshLayer : uint8_t
{
	Default = 0,
	Medium = 1,
	Large = 2,
	ESPNavMeshLayer_MAX = 3
};

// Object: Enum Feature_SP.ESPGameBossForm
enum class ESPGameBossForm : uint8_t
{
	NoneForm = 0,
	LittleTurtleForm = 1,
	GiantisedForm = 2,
	BigTurtleAndIceSnakeForm = 3,
	BigTurtleAndWaterSnakeForm = 4,
	SpecialForm1 = 5,
	SpecialForm2 = 6,
	SpecialForm3 = 7,
	SpecialForm4 = 8,
	SpecialForm5 = 9,
	ESPGameBossForm_MAX = 10
};

// Object: Enum Feature_SP.ESPStarPalVisionField
enum class ESPStarPalVisionField : uint8_t
{
	Invalid = 0,
	InSightNormal = 1,
	InSightRush = 2,
	OutOfSightWalk = 3,
	OutOfSightRush = 4,
	ESPStarPalVisionField_MAX = 5
};

// Object: Enum Feature_SP.EBodyPart
enum class EBodyPart : uint8_t
{
	NONE = 0,
	HEAD = 1,
	SNAKE = 2,
	EBodyPart_MAX = 3
};

// Object: Enum Feature_SP.ESPInteractStatus
enum class ESPInteractStatus : uint8_t
{
	None = 0,
	Conversation = 1,
	Stroke = 2,
	Feed = 3,
	Lift = 4,
	Grip = 5,
	Throw = 6,
	PreInteract = 7,
	AffinityOne = 8,
	AffinityTwo = 9,
	AffinityThree = 10,
	Lift_Begin = 11,
	Lift_End = 12,
	Lift_Interrupt = 13,
	Throw_Begin = 14,
	Relieve = 15,
	ESPInteractStatus_MAX = 16
};

// Object: Enum Feature_SP.ESPDynamicMonsterBodyShapeStatus
enum class ESPDynamicMonsterBodyShapeStatus : uint8_t
{
	None = 0,
	NormalBodySize = 1,
	EnlargedBodySize = 2,
	RefucedBodySize = 3,
	PresetBodySizeStatusOne = 4,
	PresetBodySizeStatusTwo = 5,
	PresetBodySizeStatusThree = 6,
	PresetBodySizeStatusFour = 7,
	ESPDynamicMonsterBodyShapeStatus_MAX = 8
};

// Object: Enum Feature_SP.ESPMonsterControlMode
enum class ESPMonsterControlMode : uint8_t
{
	AI = 0,
	Riding = 1,
	Morphing = 2,
	ESPMonsterControlMode_MAX = 3
};

// Object: Enum Feature_SP.ESPMonsterSize
enum class ESPMonsterSize : uint16_t
{
	Small = 0,
	Middle = 1,
	Big = 2,
	InValid = 255,
	ESPMonsterSize_MAX = 256
};

// Object: Enum Feature_SP.ESPInputActionType
enum class ESPInputActionType : uint8_t
{
	Default = 0,
	Move = 1,
	Skill = 2,
	Capture = 3,
	Tumbling = 4,
	Unleash = 5,
	Leash = 6,
	PetCommand = 7,
	Attack = 8,
	Weapon = 9,
	Rider = 10,
	Jump = 11,
	Glide = 12,
	FriendSkill = 13,
	Max = 64
};

// Object: Enum Feature_SP.ESPLandingState
enum class ESPLandingState : uint8_t
{
	None = 0,
	Landing = 1,
	Landed = 2,
	ESPLandingState_MAX = 3
};

// Object: Enum Feature_SP.ETumblingDirection
enum class ETumblingDirection : uint8_t
{
	Default = 0,
	Forward = 1,
	Left = 2,
	Back = 3,
	Right = 4,
	ETumblingDirection_MAX = 5
};

// Object: Enum Feature_SP.ESPJumpState
enum class ESPJumpState : uint8_t
{
	None = 0,
	JumpStart = 1,
	JumpLoop = 2,
	JumpEnd = 3,
	ESPJumpState_MAX = 4
};

// Object: Enum Feature_SP.ESPLocomotionState
enum class ESPLocomotionState : uint8_t
{
	GroundMove = 0,
	Jump = 1,
	Falling = 2,
	Landing = 3,
	Tumbling = 4,
	ESPLocomotionState_MAX = 5
};

// Object: Enum Feature_SP.ESPActionState
enum class ESPActionState : uint16_t
{
	SP_Begin = 0,
	SP_Aiming = 1,
	SP_FriendSkill = 2,
	SP_SkillChanneling = 3,
	SP_SkillPlaying = 4,
	SP_Constructing = 5,
	SP_Crafting = 6,
	SP_Clim = 7,
	SP_Swim = 8,
	SP_Ride = 9,
	SP_Fly = 10,
	SP_FeedPal = 11,
	SP_Stroke = 12,
	SP_MountPal = 13,
	SP_DismountPal = 14,
	SP_PalTakeOff = 15,
	SP_Stiff = 16,
	SP_Stun = 17,
	SP_LiftPal = 18,
	SP_GripPal = 19,
	SP_ThrowPal = 20,
	SP_UnLeashPet = 21,
	SP_LeashPet = 22,
	SP_WeaponFiring = 23,
	SP_SpecialAttack = 24,
	SP_ConversationPal = 25,
	SP_WeaponSwitching = 26,
	SP_WeaponReloading = 27,
	SP_Hit_Light = 28,
	SP_Dead = 29,
	SP_Freeze = 30,
	SP_Dizziness = 31,
	SP_AffinityOne = 32,
	SP_AffinityTwo = 33,
	SP_AffinityThree = 34,
	SP_GoBackCity = 35,
	SP_Pick = 36,
	SP_Visit = 37,
	SP_Rescue = 38,
	SP_Rebirth = 39,
	SP_Player_Dying = 40,
	SP_Player_GetUp = 41,
	SP_Relieve = 42,
	SP_SkillControl = 43,
	SP_PreFriendSkill = 44,
	SP_Recall = 45,
	SP_MorphBegin = 46,
	SP_MorphEnd = 47,
	SP_Monster_Idle = 48,
	SP_Monster_Hit_Back = 49,
	SP_Monster_Hit_Fly = 50,
	SP_Monster_Dying = 51,
	SP_Monster_Rescue = 52,
	SP_Monster_Attack = 53,
	SP_Monster_CounterAttack = 54,
	SP_Monster_SlotSkill = 55,
	SP_Monster_MorphBegin = 56,
	SP_Monster_MorphEnd = 57,
	SP_Invalid = 254,
	SP_End = 255,
	SP_MAX = 256
};

// Object: Enum Feature_SP.ESPMonsterSpawnReason
enum class ESPMonsterSpawnReason : uint8_t
{
	None = 0,
	SOCWork = 1,
	FollowMaster = 2,
	Battle = 3,
	ESPMonsterSpawnReason_MAX = 4
};

// Object: Enum Feature_SP.ESPMotionState
enum class ESPMotionState : uint16_t
{
	None = 0,
	ClimbBegin = 1,
	Climb = 2,
	SP_SwimInWater = 3,
	SP_Tumbling = 4,
	SP_RidePal = 5,
	SP_ComeDownLight = 6,
	SP_ComeDownHeavy = 7,
	SP_Gliding = 8,
	SP_GlidingEnd = 9,
	SP_Tumbling_Sliding = 10,
	SP_Hit_Back = 11,
	SP_Hit_Fly = 12,
	SP_UltimateMove = 13,
	SP_StopSprint = 14,
	SP_MorphHide = 15,
	SP_TakePhoto = 16,
	SP_Monster_Idle = 17,
	SP_Monster_Ground_Sleep = 18,
	SP_Monster_Flying_Enter = 19,
	SP_Monster_Flying_Idle = 20,
	SP_Monster_Flying_Interact = 21,
	SP_Monster_Flying_Battle = 22,
	SP_Monster_Flying_Skill = 23,
	SP_Monster_Flying_Death = 24,
	SP_Monster_Flying_Exit = 25,
	SP_Monster_Swimming_Enter = 26,
	SP_Monster_Swimming_Idle = 27,
	SP_Monster_Swimming_Battle = 28,
	SP_Monster_Swimming_Death = 29,
	SP_Monster_Swimming_Exit = 30,
	SP_Monster_Flying_Weak = 31,
	SP_Monster_Tumbling = 32,
	SP_Monster_Jump = 33,
	SP_Monster_Swimming_Drown = 34,
	SP_Monster_Swimming_ShallowWater = 35,
	SP_Monster_UltimateMove = 36,
	SP_Monster_Interact_Lifted = 37,
	SP_Monster_Interact_Gripped = 38,
	SP_Monster_Interact_Thrown = 39,
	SP_Monster_Morph = 40,
	SP_Monster_LandingLight = 41,
	SP_Monster_LandingHeavy = 42,
	SP_Monster_MorphUltimateSkill = 43,
	SP_Monster_Floating = 44,
	SP_Monster_MorphGliding = 45,
	SP_Monster_MorphGlidingEnd = 46,
	SP_Monster_SpecialJump = 47,
	SP_Monster_End = 48,
	SP_Invalid = 254,
	SP_End = 255,
	ESPMotionState_MAX = 256
};

// Object: Enum Feature_SP.ESPSignificanceLevel
enum class ESPSignificanceLevel : uint8_t
{
	Default = 0,
	Extreme = 1,
	Ultra = 2,
	High = 3,
	Medium = 4,
	Low = 5,
	OutsideExtreme = 6,
	OutsideUltra = 7,
	OutsideHigh = 8,
	OutsideMedium = 9,
	OutsideLow = 10,
	Max = 11
};

// Object: Enum Feature_SP.ESPBroadcastAttackNotifyDirection
enum class ESPBroadcastAttackNotifyDirection : uint8_t
{
	Invalid = 0,
	Forward = 1,
	Left = 2,
	Right = 3,
	ESPBroadcastAttackNotifyDirection_MAX = 4
};

// Object: Enum Feature_SP.ESPSkillStage
enum class ESPSkillStage : uint8_t
{
	None = 0,
	Start = 1,
	StartLoop = 2,
	Action = 3,
	ActionLoop = 4,
	End = 5,
	Max = 6
};

// Object: Enum Feature_SP.ESPAbilityRelationType
enum class ESPAbilityRelationType : uint8_t
{
	Self = 0,
	Ally = 1,
	Enemy = 2,
	Unrelated = 3,
	ESPAbilityRelationType_MAX = 4
};

// Object: Enum Feature_SP.EStateInterceptorType
enum class EStateInterceptorType : uint8_t
{
	None = 0,
	Sprint = 1,
	Burden = 2,
	EStateInterceptorType_MAX = 3
};

// Object: Enum Feature_SP.ESPRideSeatType
enum class ESPRideSeatType : uint8_t
{
	Invalid = 0,
	Driver = 1,
	Passanger = 2,
	Any = 3,
	ESPRideSeatType_MAX = 4
};

// Object: Enum Feature_SP.ESPSignificanceRunMode
enum class ESPSignificanceRunMode : uint8_t
{
	None = 0,
	Client = 1,
	Server = 2,
	Standalone = 3,
	ESPSignificanceRunMode_MAX = 4
};

// Object: Enum Feature_SP.ESPShootRotationType
enum class ESPShootRotationType : uint8_t
{
	UseSelfRotation = 0,
	UseShootDirection = 1,
	UseTargetPosition = 2,
	UsePlayerAimingPoint = 3,
	UseCrosshairDirection = 4,
	ESPShootRotationType_MAX = 5
};

// Object: Enum Feature_SP.ESPShootType
enum class ESPShootType : uint8_t
{
	Manual = 0,
	Auto = 1,
	Timed = 2,
	ESPShootType_MAX = 3
};

// Object: Enum Feature_SP.EProjectileNetPolicy
enum class EProjectileNetPolicy : uint8_t
{
	RepMovement = 0,
	Independent = 1,
	LocalOnly = 2,
	AllClient = 3,
	EProjectileNetPolicy_MAX = 4
};

// Object: Enum Feature_SP.ESPTraceType
enum class ESPTraceType : uint8_t
{
	None = 0,
	Gameplay = 1,
	Weapon = 2,
	Pawn = 3,
	WorldStatic = 4,
	Floor = 5,
	Climbable = 6,
	CaptureBall = 7,
	CaptureTrace = 8,
	CallMonster = 9,
	WildMonster = 10,
	GameBuilding = 11,
	GameProps = 12,
	Choppable = 13,
	BlockBuilding = 14,
	ActualMesh = 15,
	PackageAirWall = 16,
	InterWater = 17,
	AirWall = 18,
	GameStaticBuilding = 19,
	CallMonsterMesh = 20,
	WildMonsterMesh = 21,
	OWBossAirWall = 22,
	ESPTraceType_MAX = 23
};

// Object: Enum Feature_SP.ESPProjectileDistRangeType
enum class ESPProjectileDistRangeType : uint8_t
{
	TargetActor = 0,
	ForwardActor = 1,
	ESPProjectileDistRangeType_MAX = 2
};

// Object: Enum Feature_SP.ESPProjectileBezierFinishSpeedDirType
enum class ESPProjectileBezierFinishSpeedDirType : uint8_t
{
	OrigShootDir = 0,
	CurSpeedDir = 1,
	ESPProjectileBezierFinishSpeedDirType_MAX = 2
};

// Object: Enum Feature_SP.ESPProjectileBezierDestType
enum class ESPProjectileBezierDestType : uint8_t
{
	MannualSpecified = 0,
	TargetActor = 1,
	WeaponRange = 2,
	RayCast = 3,
	ESPProjectileBezierDestType_MAX = 4
};

// Object: Enum Feature_SP.ESPProjectileVFXLocationType
enum class ESPProjectileVFXLocationType : uint8_t
{
	SelfLocation = 0,
	ImpactPoint = 1,
	HitActorLocation = 2,
	OwnerActorLocation = 3,
	ESPProjectileVFXLocationType_MAX = 4
};

// Object: Enum Feature_SP.ESPProjectileTrajectoryType
enum class ESPProjectileTrajectoryType : uint8_t
{
	Straight = 0,
	Parabola = 1,
	Reversible = 2,
	BezierCurve = 3,
	Revolve = 4,
	StraightFixedTime = 5,
	ESPProjectileTrajectoryType_MAX = 6
};

// Object: Enum Feature_SP.ESPAimingState
enum class ESPAimingState : uint8_t
{
	None = 0,
	AimingStart = 1,
	CameraFadeIn = 2,
	AimingReady = 3,
	Firing = 4,
	AimingCancel = 5,
	CameraFadeOut = 6,
	AimingEnd = 7,
	ESPAimingState_MAX = 8
};

// Object: Enum Feature_SP.ECubicLockedSourceType
enum class ECubicLockedSourceType : uint8_t
{
	None = 0,
	Monster = 1,
	Ability = 2,
	ECubicLockedSourceType_MAX = 3
};

// Object: Enum Feature_SP.ESPLookAtInterpolationType
enum class ESPLookAtInterpolationType : uint8_t
{
	Linear = 0,
	Cubic = 1,
	Sinusoidal = 2,
	EaseInOutExponent2 = 3,
	EaseInOutExponent3 = 4,
	EaseInOutExponent4 = 5,
	EaseInOutExponent5 = 6,
	ESPLookAtInterpolationType_MAX = 7
};

// Object: Enum Feature_SP.ESPBFCondtionType
enum class ESPBFCondtionType : uint8_t
{
	Both = 0,
	OnlyDistance = 1,
	OnlyAngle = 2,
	ESPBFCondtionType_MAX = 3
};

// Object: Enum Feature_SP.ESPCheckBuffsCondition
enum class ESPCheckBuffsCondition : uint8_t
{
	All = 0,
	Contains = 1,
	None = 2,
	ESPCheckBuffsCondition_MAX = 3
};

// Object: Enum Feature_SP.ESPCharacterType
enum class ESPCharacterType : uint8_t
{
	None = 0,
	Companion = 1,
	Player = 2,
	Normal = 3,
	Elite = 4,
	Boss = 5,
	InVisible = 6,
	ESPCharacterType_MAX = 7
};

// Object: Enum Feature_SP.ESPDistanceType
enum class ESPDistanceType : uint8_t
{
	LessRange = 0,
	InRange = 1,
	GreaterRange = 2,
	OutRange = 3,
	ESPDistanceType_MAX = 4
};

// Object: Enum Feature_SP.ESPCheckTagsCondition
enum class ESPCheckTagsCondition : uint8_t
{
	All = 0,
	Contains = 1,
	None = 2,
	ESPCheckTagsCondition_MAX = 3
};

// Object: Enum Feature_SP.ECheckTaskFailedReason
enum class ECheckTaskFailedReason : uint8_t
{
	None = 0,
	BuildingPropNotProduct = 1,
	PropInValid = 2,
	MovePropInValid = 3,
	ECheckTaskFailedReason_MAX = 4
};

// Object: Enum Feature_SP.ESPActionSeqSet_EnterConditionType
enum class ESPActionSeqSet_EnterConditionType : uint8_t
{
	Any = 0,
	At = 1,
	InRange = 2,
	ESPActionSeqSet_MAX = 3
};

// Object: Enum Feature_SP.EAICombatPhase
enum class EAICombatPhase : uint8_t
{
	InCombat = 0,
	Step1 = 1,
	Step2 = 2,
	Step3 = 3,
	Step4 = 4,
	Step5 = 5,
	Step6 = 6,
	Step7 = 7,
	EAICombatPhase_MAX = 8
};

// Object: Enum Feature_SP.EExecutionType
enum class EExecutionType : uint8_t
{
	ExecutionOnlyStart = 0,
	ExecutionOnlyFinish = 1,
	ExecutionBoth = 2,
	EExecutionType_MAX = 3
};

// Object: Enum Feature_SP.EBBKEvalFlag
enum class EBBKEvalFlag : uint8_t
{
	NO_NEED_EVAL = 0,
	NEED_EVAL = 1,
	EVAL_NO_RESULT = 2,
	EVAL_TRUE_RESULT = 3,
	EVAL_FALSE_RESULT = 4,
	EBBKEvalFlag_MAX = 5
};

// Object: Enum Feature_SP.ESPCheckThreatOutOfBFType
enum class ESPCheckThreatOutOfBFType : uint8_t
{
	Default = 0,
	OnlyThreat = 1,
	AllPlayer = 2,
	LevelMonsterSummoner = 3,
	ESPCheckThreatOutOfBFType_MAX = 4
};

// Object: Enum Feature_SP.ESPUpdateCombatTargetType
enum class ESPUpdateCombatTargetType : uint8_t
{
	Default = 0,
	Type1 = 1,
	ESPUpdateCombatTargetType_MAX = 2
};

// Object: Enum Feature_SP.EConditionalSkillConditionType
enum class EConditionalSkillConditionType : uint8_t
{
	None = 0,
	AngleCheck = 1,
	EConditionalSkillConditionType_MAX = 2
};

// Object: Enum Feature_SP.ESPPickRule
enum class ESPPickRule : uint8_t
{
	Random = 0,
	Closest = 1,
	Furthest = 2,
	LowestHP = 3,
	HighestHP = 4,
	ESPPickRule_MAX = 5
};

// Object: Enum Feature_SP.ESPFilterRule_ActorType
enum class ESPFilterRule_ActorType : uint8_t
{
	None = 0,
	PlayerClass = 1,
	CompanionClass = 2,
	SummonsOnly = 3,
	ESPFilterRule_MAX = 4
};

// Object: Enum Feature_SP.ESPFilterRule_Buff
enum class ESPFilterRule_Buff : uint8_t
{
	None = 0,
	HasBuffId = 1,
	ESPFilterRule_MAX = 2
};

// Object: Enum Feature_SP.ESPFilterRule_HP
enum class ESPFilterRule_HP : uint8_t
{
	None = 0,
	HPRatioBelow = 1,
	HPRatioAbove = 2,
	ESPFilterRule_MAX = 3
};

// Object: Enum Feature_SP.ESPFilterRule_Distance
enum class ESPFilterRule_Distance : uint8_t
{
	None = 0,
	TooClose = 1,
	InRange = 2,
	OutOfRange = 3,
	ESPFilterRule_MAX = 4
};

// Object: Enum Feature_SP.ESPAIInteractionType
enum class ESPAIInteractionType : uint8_t
{
	Invalid = 0,
	Touch = 1,
	Feeding = 2,
	Feeding2 = 3,
	Conversation = 4,
	InteractionType1 = 5,
	InteractionType2 = 6,
	InteractionType3 = 7,
	MasterLogin = 8,
	ESPAIInteractionType_MAX = 9
};

// Object: Enum Feature_SP.ESPPlaySkillSlotPhase
enum class ESPPlaySkillSlotPhase : uint8_t
{
	Invalid = 0,
	PreMove = 1,
	MoveToTarget = 2,
	PreSkill = 3,
	PlaySkill = 4,
	Done = 5,
	ESPPlaySkillSlotPhase_MAX = 6
};

// Object: Enum Feature_SP.ESPMonsterRotateSpeedType
enum class ESPMonsterRotateSpeedType : uint8_t
{
	Normal = 0,
	Slow = 1,
	Medium = 2,
	Fast = 3,
	ESPMonsterRotateSpeedType_MAX = 4
};

// Object: Enum Feature_SP.ESPSetActionSeqIdOp
enum class ESPSetActionSeqIdOp : uint8_t
{
	None = 0,
	Goto = 1,
	ApplyOffset = 2,
	ESPSetActionSeqIdOp_MAX = 3
};

// Object: Enum Feature_SP.ESPSetupCombatSkillSet_FilterRule
enum class ESPSetupCombatSkillSet_FilterRule : uint8_t
{
	Default = 0,
	NonExclusiveSkill = 1,
	ESPSetupCombatSkillSet_MAX = 2
};

// Object: Enum Feature_SP.EMonsterExpressionType
enum class EMonsterExpressionType : uint8_t
{
	Cute = 0,
	Perceive = 1,
	Curious = 2,
	EMonsterExpressionType_MAX = 3
};

// Object: Enum Feature_SP.E_SP_GpoTriggerEventType
enum class E_SP_GpoTriggerEventType : uint8_t
{
	Out = 0,
	Triggered = 1,
	E_SP_MAX = 2
};

// Object: Enum Feature_SP.ESPGameType
enum class ESPGameType : uint16_t
{
	SPLobbyGame = 10401,
	SPGame = 10402,
	SPDungeonGame = 10403,
	SPGroupGame = 10404,
	SPPvpGame = 10405,
	ESPGameType_MAX = 10406
};

// Object: Enum Feature_SP.ESPCommandState
enum class ESPCommandState : uint8_t
{
	None = 0,
	Received = 1,
	Success = 2,
	Failed = 3,
	ESPCommandState_MAX = 4
};

// Object: Enum Feature_SP.EGenericFSMLogLevel
enum class EGenericFSMLogLevel : uint8_t
{
	Log = 0,
	Warning = 1,
	Error = 2,
	EGenericFSMLogLevel_MAX = 3
};

// Object: Enum Feature_SP.EGenericFSMDomain
enum class EGenericFSMDomain : uint16_t
{
	ClientOnly = 0,
	ServerAuthoritative = 1,
	Auto = 255,
	EGenericFSMDomain_MAX = 256
};

// Object: Enum Feature_SP.EGenericFSMTransitionFailureReason
enum class EGenericFSMTransitionFailureReason : uint16_t
{
	None = 0,
	InvalidTargetState = 1,
	NoEnterRuleMap = 2,
	TransitionBlocked = 3,
	CanEnterCheckFailed = 4,
	InvalidStateId = 5,
	NoAuthority = 6,
	Unknown = 255,
	EGenericFSMTransitionFailureReason_MAX = 256
};

// Object: Enum Feature_SP.EGenericFSMStateTransitionType
enum class EGenericFSMStateTransitionType : uint16_t
{
	Blocked = 0,
	Allowed = 1,
	Interrupted = 2,
	Invalid = 255,
	EGenericFSMStateTransitionType_MAX = 256
};

// Object: Enum Feature_SP.ESPLevelInfoType
enum class ESPLevelInfoType : uint8_t
{
	CommonInfo = 0,
	Treasure = 1,
	POIInfo = 2,
	Max = 3
};

// Object: Enum Feature_SP.ESPCameraBattleAssistCtrlEndReason
enum class ESPCameraBattleAssistCtrlEndReason : uint8_t
{
	Normal = 0,
	PlayerInput = 1,
	ESPCameraBattleAssistCtrlEndReason_MAX = 2
};

// Object: Enum Feature_SP.EBattleProcessorFadeType
enum class EBattleProcessorFadeType : uint8_t
{
	NormalFight = 0,
	AddNormal = 1,
	Boss = 2,
	EBattleProcessorFadeType_MAX = 3
};

// Object: Enum Feature_SP.EMoeSPCameraFixedTargetLocationType
enum class EMoeSPCameraFixedTargetLocationType : uint8_t
{
	Invalid = 0,
	TargetPos = 1,
	Center = 2,
	EMoeSPCameraFixedTargetLocationType_MAX = 3
};

// Object: Enum Feature_SP.EMoeSPCameraAimType
enum class EMoeSPCameraAimType : uint8_t
{
	Invalid = 0,
	Normal = 1,
	RiddenFullBody = 2,
	RiddenPerson = 3,
	EMoeSPCameraAimType_MAX = 4
};

// Object: Enum Feature_SP.EMoeSPCameraAimReason
enum class EMoeSPCameraAimReason : uint8_t
{
	Invalid = 0,
	Normal = 1,
	Skill = 2,
	Weapon = 3,
	ThorwBall = 4,
	EMoeSPCameraAimReason_MAX = 5
};

// Object: Enum Feature_SP.ESPCameraCustomParamChgState
enum class ESPCameraCustomParamChgState : uint8_t
{
	InValid = 0,
	NeedDel = 1,
	CurSet = 2,
	ESPCameraCustomParamChgState_MAX = 3
};

// Object: Enum Feature_SP.EBattleAssistCtrlWeaponType
enum class EBattleAssistCtrlWeaponType : uint8_t
{
	InValid = 0,
	Meleen = 1,
	Gun = 2,
	Wand = 3,
	Crosshair = 4,
	EBattleAssistCtrlWeaponType_MAX = 5
};

// Object: Enum Feature_SP.EMoeSPInputRotationType
enum class EMoeSPInputRotationType : uint8_t
{
	DisableAll = 0,
	EnableYaw = 1,
	EnablePitch = 2,
	EnableEndSequence = 3,
	EnablePitchYaw = 4,
	EMoeSPInputRotationType_MAX = 5
};

// Object: Enum Feature_SP.EMoeSPWeakTargetSkillCancelReason
enum class EMoeSPWeakTargetSkillCancelReason : uint8_t
{
	Invalid = 0,
	NoWeakTargetAtk = 1,
	HitBreakFly = 2,
	EMoeSPWeakTargetSkillCancelReason_MAX = 3
};

// Object: Enum Feature_SP.EMoeSPCameraMainPlayerStatePriority
enum class EMoeSPCameraMainPlayerStatePriority : uint8_t
{
	Invalid = 0,
	InGame = 1,
	Lobby = 2,
	DDP = 3,
	UGC = 4,
	Home = 5,
	Farm = 6,
	Cook = 7,
	House = 8,
	CustomOverride = 9,
	Spectator = 10,
	MoeGPOOverride = 11,
	HouseEdit = 12,
	AimShootOverride = 13,
	Follow2D = 14,
	Arena_CustomMode = 15,
	UGCOverride = 16,
	UGC_BlockCodingOverride = 17,
	Arena_CustomOverride = 18,
	Arena_CameraAdjustAbleTask = 19,
	LobbyShuttlePrepare = 20,
	LobbyShuttleFly = 21,
	UnKnown = 22,
	UnKnown = 23,
	UnKnown = 24,
	UnKnown = 25,
	UnKnown = 26,
	SPGame_Default = 27,
	SPGame_BattleDefault = 28,
	SPGame_LevelCtrl1 = 29,
	SPGame_LevelCtrl2 = 30,
	SPGame_LevelCtrl3 = 31,
	SPGame_HoldRangeWeapon = 32,
	SPGame_Swim = 33,
	SPGame_Stroke = 34,
	SPGame_Feed = 35,
	SPGame_Climb = 36,
	SPGame_Riding = 37,
	SPGame_Morph = 38,
	SPGame_MorphFly = 39,
	SPGame_WeaponAim = 40,
	SPGame_FriendSkill = 41,
	SPGame_Aim = 42,
	SPGame_SkillTask = 43,
	SPGame_MorphSkill = 44,
	SPGame_SkillBossPerform = 45,
	SPGame_Test = 46,
	SPGame_ClickSOC = 47,
	SPGame_ClickSOC_GodStatue = 48,
	SPGame_PressSOC = 49,
	SPGame_LockTarget = 50,
	SPGame_NPCInteract = 51,
	SPGame_Sequence = 52,
	SPGame_BattleTeleport = 53,
	SPGame_RiddenAim = 54,
	SPGame_Photo = 55,
	EMoeSPCameraMainPlayerStatePriority_MAX = 56
};

// Object: Enum Feature_SP.EMoeSPCameraModifierPriority
enum class EMoeSPCameraModifierPriority : uint8_t
{
	Default = 0,
	Lobby = 1,
	InGame = 2,
	DDP = 3,
	UGC = 4,
	Home = 5,
	Farm = 6,
	Cook = 7,
	House = 8,
	CustomOverride = 9,
	SpectatorBack = 10,
	SpectatorForward = 11,
	SpectatorRight = 12,
	SpectatorTop = 13,
	LevelCustomOverride = 14,
	KillerCamera = 15,
	Arena_CustomMode = 16,
	HouseEdit = 17,
	Invalid_DefaultFlag = 18,
	UGCOverride = 19,
	Home_Furniture_Camera = 20,
	UGC_BlockCodingOverride = 21,
	Arena_CustomOverride = 22,
	Arena_CameraAdjustAbleTask = 23,
	LobbyShuttlePrepare = 24,
	LobbyShuttleFly = 25,
	Stun_Camera = 26,
	Photo = 27,
	UnKnown = 28,
	UnKnown = 29,
	UnKnown = 30,
	UnKnown = 31,
	UnKnown = 32,
	SPGame_Default = 33,
	SPGame_BattleDefault = 34,
	SPGame_LevelCtrl1 = 35,
	SPGame_LevelCtrl2 = 36,
	SPGame_LevelCtrl3 = 37,
	SPGame_HoldRangeWeapon = 38,
	SPGame_Swim = 39,
	SPGame_Stroke = 40,
	SPGame_Feed = 41,
	SPGame_Climb = 42,
	SPGame_Riding = 43,
	SPGame_Morph = 44,
	SPGame_MorphFly = 45,
	SPGame_WeaponAim = 46,
	SPGame_FriendSkill = 47,
	SPGame_Aim = 48,
	SPGame_SkillTask = 49,
	SPGame_MorphSkill = 50,
	SPGame_SkillBossPerform = 51,
	SPGame_Test = 52,
	SPGame_ClickSOC = 53,
	SPGame_ClickSOC_GodStatue = 54,
	SPGame_PressSOC = 55,
	SPGame_LockTarget = 56,
	SPGame_NPCInteract = 57,
	SPGame_BattleTeleport = 58,
	SPGame_RiddenAim = 59,
	SPGame_Photo = 60,
	EMoeSPCameraModifierPriority_MAX = 61
};

// Object: Enum Feature_SP.EMoeSPCameraDataProcessorPriority
enum class EMoeSPCameraDataProcessorPriority : uint8_t
{
	Modifier_Processor = 1,
	RotationInput_Processor = 2,
	RotAdapt_Processor = 11,
	SurviveGameLocBias_Processor = 12,
	RaceGameLocBias_Processor = 13,
	ArmLengthWithPitch_Processor = 21,
	LocationLag_Processor = 51,
	SPGameLag_Processor = 52,
	SPSphericMotionProcessor = 53,
	SPRotationModifier_Processor = 54,
	SPSOCPressModifier_Processor = 55,
	SPLockTarget_Processor = 56,
	SPClickSOC_Processor = 57,
	SPPitchReset_Processor = 58,
	SPBattle_Processor = 59,
	SPBattleTurn_Processor = 60,
	SocketZAdjust_Processor = 61,
	SPYawReset_Processor = 62,
	SPInteractTurn_Processor = 63,
	SPTargetAssistCtrl_Processor = 64,
	SPBattleAssistCtrl_Processor = 65,
	SPTargetOffsetProcessor = 66,
	SPRotateToTargetProcessor = 67,
	LockTarget_Processor = 71,
	Follow2D_Processor = 81,
	SPCameraSequence_Processor = 91,
	SPPosAftercure_Processor = 92,
	EMoeSPCameraDataProcessorPriority_MAX = 93
};

// Object: Enum Feature_SP.EMoeSPCameraManagerStatePriority
enum class EMoeSPCameraManagerStatePriority : uint8_t
{
	Moe_MainPlayer_Camera = 0,
	Moe_Community_NPC_Camera = 1,
	Moe_Community_Preview_Camera = 2,
	Moe_Community_Lottery_Camera = 3,
	Moe_Community_GoldSuit_Camera = 4,
	Moe_Community_FaceEmotion_Camera = 5,
	Moe_ShootGame_1P_Camera = 6,
	Moe_ShootGame_3P_Camera = 7,
	Moe_ShootGame_Dead_Camera = 8,
	Moe_Community_GoldSuit_Camera2 = 9,
	Moe_Community_PlayFirework_Camera = 10,
	Moe_SnowboardGame_Camera = 11,
	Moe_Dialogue_Camera = 12,
	Moe_Custom_Camera = 13,
	Moe_DDP_Begin_Camera = 14,
	Moe_Vehicle = 15,
	Moe_MCG_Camera = 16,
	Moe_Community_LevelSequence = 17,
	Moe_MAY_FollowCamera = 18,
	Moe_Simple_Move = 19,
	Moe_NR3E8_Camera = 20,
	Moe_UGC_EGame_Camera = 21,
	Moe_UGC_EGod_Camera = 22,
	Moe_UGC_EPhoto_Camera = 23,
	Moe_UGC_Fixed_Camera = 24,
	Moe_UGC_Static_Camera = 25,
	Moe_UGC_Spline_Camera = 26,
	Moe_UGC_DynamicPrevew_Camera = 27,
	Moe_UGC_AvatarPreview_Camera = 28,
	Moe_UGC_Preview_Camera = 29,
	Moe_Home_TripCamera = 30,
	Moe_UGC_EPhotoInPlay_Camera = 31,
	Moe_Arena_OverLook_Camera = 32,
	Moe_Arena_Traditional_Camera = 33,
	Moe_COC_TopDown_Camera = 34,
	Moe_COC_Simple_Move = 35,
	Moe_COC_Camera_LUA1 = 36,
	Moe_COC_Camera_LUA2 = 37,
	Moe_COC_Camera_LUA3 = 38,
	Moe_COC_Camera_LUA4 = 39,
	Moe_COC_LevelSequence_Camera = 40,
	Moe_Hotfix_Camera1 = 41,
	Moe_Hotfix_Camera2 = 42,
	Moe_Hotfix_Camera3 = 43,
	Moe_Hotfix_Camera4 = 44,
	Moe_SP_Fixed_Camera = 45,
	Moe_SP_PVP_Camera = 46,
	Moe_SP_Tripod_Camera = 47,
	Moe_CameraState_Max = 48,
	Moe_MAX = 49
};

// Object: Enum Feature_SP.EMoeSPPosAftercureFlags
enum class EMoeSPPosAftercureFlags : uint8_t
{
	None = 0,
	Ride = 1,
	Morph = 2,
	All = 4,
	EMoeSPPosAftercureFlags_MAX = 5
};

// Object: Enum Feature_SP.ESPCameraTargetAssistState
enum class ESPCameraTargetAssistState : uint8_t
{
	None = 0,
	WeakTarget = 1,
	LockTarget = 2,
	ESPCameraTargetAssistState_MAX = 3
};

// Object: Enum Feature_SP.EAimTargetLocationType
enum class EAimTargetLocationType : uint8_t
{
	AimTowardSTP = 0,
	AimTowardTargetLocation = 1,
	EAimTargetLocationType_MAX = 2
};

// Object: Enum Feature_SP.EProjectileTargetTeamModeType
enum class EProjectileTargetTeamModeType : uint8_t
{
	None = 0,
	SameTeam = 1,
	DifferentTeam = 2,
	EProjectileTargetTeamModeType_MAX = 3
};

// Object: Enum Feature_SP.ECaptureFunctionType
enum class ECaptureFunctionType : uint8_t
{
	Capture = 0,
	Unleash = 1,
	Leash = 2,
	ECaptureFunctionType_MAX = 3
};

// Object: Enum Feature_SP.EAirWallType
enum class EAirWallType : uint8_t
{
	Normal = 0,
	Package = 1,
	Dungeon = 2,
	DungeonBoss = 3,
	OpenWorld = 4,
	OpenWorldBoss = 5,
	EAirWallType_MAX = 6
};

// Object: Enum Feature_SP.EAppearTypeInLevel
enum class EAppearTypeInLevel : uint8_t
{
	Default = 0,
	AncientBoss1 = 1,
	AncientBoss2 = 2,
	AncientBoss3 = 3,
	AncientBoss4 = 4,
	EAppearTypeInLevel_MAX = 5
};

// Object: Enum Feature_SP.EBossBodySizeType
enum class EBossBodySizeType : uint8_t
{
	Default = 0,
	Small = 1,
	Middle = 2,
	Big = 3,
	AncientBoss_ZhuQue = 4,
	AncientBoss_XuanWu = 5,
	AncientBoss_BaiHu = 6,
	Common_Small = 7,
	Common_Middle = 8,
	Common_Big = 9,
	AncientBoss_QingLong = 10,
	EBossBodySizeType_MAX = 11
};

// Object: Enum Feature_SP.EGPOType
enum class EGPOType : uint8_t
{
	Default = 0,
	LowLevelTreasureChest = 1,
	MiddleLevelTreasureChest = 2,
	HighLevelTreasureChest = 3,
	LowLevelChoppable = 4,
	MiddleLevelChoppable = 5,
	HighLevelChoppable = 6,
	NPCType = 7,
	EGPOType_MAX = 8
};

// Object: Enum Feature_SP.EVirtualPointMapping
enum class EVirtualPointMapping : uint8_t
{
	Default = 0,
	Object = 1,
	EVirtualPointMapping_MAX = 2
};

// Object: Enum Feature_SP.EAILevel
enum class EAILevel : uint8_t
{
	Default = 0,
	Normal = 1,
	Elite = 2,
	Boss = 3,
	Special = 4,
	EAILevel_MAX = 5
};

// Object: Enum Feature_SP.ESTPAIState
enum class ESTPAIState : uint8_t
{
	Casual = 0,
	Fighting = 1,
	ESTPAIState_MAX = 2
};

// Object: Enum Feature_SP.EAIMoveLoopType
enum class EAIMoveLoopType : uint8_t
{
	LeaveWhenEnd = 0,
	RoundTripLoop = 1,
	MoveInCircles = 2,
	EAIMoveLoopType_MAX = 3
};

// Object: Enum Feature_SP.EAIMoveType
enum class EAIMoveType : uint8_t
{
	Walk = 0,
	Run = 1,
	GPOMove = 2,
	EAIMoveType_MAX = 3
};

// Object: Enum Feature_SP.EDungeonType
enum class EDungeonType : uint8_t
{
	Underland = 0,
	Maze = 1,
	Tower = 2,
	MultPlayerPVE = 3,
	RLTower = 4,
	Free = 5,
	PVP = 6,
	ClimbTower = 7,
	PeriodClimbTower = 8,
	EDungeonType_MAX = 9
};

// Object: Enum Feature_SP.E_SP_GpoOperateCompareType
enum class E_SP_GpoOperateCompareType : uint8_t
{
	CLOSE = 0,
	EQUAL = 1,
	NOTEQUAL = 2,
	GREATER = 3,
	LESS = 4,
	E_SP_MAX = 5
};

// Object: Enum Feature_SP.E_SP_GpoOperateEventType
enum class E_SP_GpoOperateEventType : uint8_t
{
	OPERATE_START = 0,
	OPERATE_COMPLETE = 1,
	OPERATE_READY = 2,
	OPERATE_MAX = 3
};

// Object: Enum Feature_SP.ESP_PalHeartEmoteType
enum class ESP_PalHeartEmoteType : uint8_t
{
	Normal = 0,
	Sleep = 1,
	Happy = 2,
	Sad = 3,
	Angry = 4,
	Tired = 5,
	Dying = 6,
	Special = 7,
	Max = 8,
	Empty = 9
};

// Object: Enum Feature_SP.ESP_PalMouthEmoteType
enum class ESP_PalMouthEmoteType : uint8_t
{
	Normal = 0,
	Happy = 1,
	Sad = 2,
	Angry = 3,
	Max = 4,
	Empty = 5
};

// Object: Enum Feature_SP.ESP_PalEmoteType
enum class ESP_PalEmoteType : uint8_t
{
	OpenEyes = 0,
	CloseEyes = 1,
	Happy = 2,
	Sad = 3,
	Angry = 4,
	Tired = 5,
	Dying = 6,
	Max = 7,
	Empty = 8
};

// Object: Enum Feature_SP.ESPPropSpawnType
enum class ESPPropSpawnType : uint8_t
{
	Default = 0,
	Player = 1,
	POI = 2,
	PVETreasureChest = 3,
	ESPPropSpawnType_MAX = 4
};

// Object: Enum Feature_SP.ESP_SignificanceShortClassID
enum class ESP_SignificanceShortClassID : uint8_t
{
	Default = 0,
	Character = 1,
	Monster = 2,
	Summon = 3,
	Projectile = 4,
	POI = 5,
	Prop = 6,
	Building = 7,
	Count = 8,
	ESP_MAX = 9
};

// Object: Enum Feature_SP.ETreasureChestOpenStatus
enum class ETreasureChestOpenStatus : uint8_t
{
	UnOpened = 0,
	Opening = 1,
	Opened = 2,
	ETreasureChestOpenStatus_MAX = 3
};

// Object: Enum Feature_SP.EWorkAttrType
enum class EWorkAttrType : uint8_t
{
	Unknown = 0,
	Build = 1,
	Handwork = 2,
	Wood = 3,
	Rock = 4,
	Max = 5
};

// Object: Enum Feature_SP.EWorkValueType
enum class EWorkValueType : uint8_t
{
	Float = 0,
	Boolean = 1,
	Integer = 2,
	EWorkValueType_MAX = 3
};

// Object: Enum Feature_SP.ESPAbilityDamageEffectType
enum class ESPAbilityDamageEffectType : uint8_t
{
	Client_HighPing = 0,
	Client_Local = 1,
	Client_Report = 2,
	Server_Receive = 3,
	Server_Only = 4,
	ESPAbilityDamageEffectType_MAX = 5
};

// Object: Enum Feature_SP.ESPModifyOperator
enum class ESPModifyOperator : uint8_t
{
	Additive = 0,
	Multiplicitive = 1,
	Division = 2,
	Override = 3,
	Max = 4,
	Min = 5,
	Append = 6,
	Remove = 7,
	Invalid = 8
};

// Object: Enum Feature_SP.ESPAbilityObjectType
enum class ESPAbilityObjectType : uint8_t
{
	Actor = 0,
	PetInBackpack = 1,
	ESPAbilityObjectType_MAX = 2
};

// Object: Enum Feature_SP.ESPAbilitySerializeReason
enum class ESPAbilitySerializeReason : uint8_t
{
	NoWhy = 0,
	ActorToNonEntity = 1,
	NonEntityToActor = 2,
	ESPAbilitySerializeReason_MAX = 3
};

// Object: Enum Feature_SP.ESPAbilityDrawDebugMode
enum class ESPAbilityDrawDebugMode : uint8_t
{
	None = 0,
	DedicatedServer = 1,
	Client = 2,
	Both = 3,
	ESPAbilityDrawDebugMode_MAX = 4
};

// Object: Enum Feature_SP.EAbilityFeatureCategory
enum class EAbilityFeatureCategory : uint8_t
{
	Consume = 0,
	EventListener = 1,
	Condition = 2,
	Action = 3,
	EAbilityFeatureCategory_MAX = 4
};

// Object: Enum Feature_SP.ESPVariableBaseType
enum class ESPVariableBaseType : uint8_t
{
	Integer = 0,
	Float = 1,
	String = 2,
	Boolean = 3,
	Structure = 4,
	StringArray = 5,
	ObjectReference = 6,
	ESPVariableBaseType_MAX = 7
};

// Object: Enum Feature_SP.ESPAxisMode
enum class ESPAxisMode : uint8_t
{
	StandardAxis = 0,
	CustomAxis = 1,
	ESPAxisMode_MAX = 2
};

// Object: Enum Feature_SP.ESPAimingRuleParamsType
enum class ESPAimingRuleParamsType : uint8_t
{
	ReadFromPreset = 0,
	Customized = 1,
	ESPAimingRuleParamsType_MAX = 2
};

// Object: Enum Feature_SP.ESPTargetActorMode
enum class ESPTargetActorMode : uint8_t
{
	TargetActorLocation = 0,
	TargetActorHead = 1,
	TargetCustomizedSocket = 2,
	ESPTargetActorMode_MAX = 3
};

// Object: Enum Feature_SP.ESPAimingTargetingMode
enum class ESPAimingTargetingMode : uint8_t
{
	NoTarget = 0,
	TargetActor = 1,
	TargetLocation = 2,
	ESPAimingTargetingMode_MAX = 3
};

// Object: Enum Feature_SP.ESPIsSelectableTargetErrCode
enum class ESPIsSelectableTargetErrCode : uint8_t
{
	Unknown = 0,
	ValidTarget = 1,
	InvalidActor = 2,
	Dead = 3,
	Hidden = 4,
	DuringCapture = 5,
	InvisibleBuff = 6,
	NoSearchTag = 7,
	AILOD_NoTick = 8,
	ESPIsSelectableTargetErrCode_MAX = 9
};

// Object: Enum Feature_SP.ESPAILogicProfile
enum class ESPAILogicProfile : uint8_t
{
	Default = 0,
	BOSS_NT = 1,
	BOSS_WT = 2,
	PH_03 = 3,
	PH_04 = 4,
	PH_05 = 5,
	ESPAILogicProfile_MAX = 6
};

// Object: Enum Feature_SP.ESPFactionRelationType
enum class ESPFactionRelationType : uint8_t
{
	Undefined = 0,
	Ally = 1,
	Friendly = 2,
	Neutral = 3,
	Hostile = 4,
	ESPFactionRelationType_MAX = 5
};

// Object: Enum Feature_SP.ESPMonsterType
enum class ESPMonsterType : uint8_t
{
	Unknown = 0,
	Wildlife = 1,
	Domestic = 2,
	Companion = 3,
	ESPMonsterType_MAX = 4
};

// Object: Enum Feature_SP.ESPGrandAIState
enum class ESPGrandAIState : uint8_t
{
	Invalid = 0,
	DeathState = 1,
	CombatState = 2,
	AlertState = 3,
	RelaxState = 4,
	ESPGrandAIState_MAX = 5
};

// Object: Enum Feature_SP.EMoveSpeed
enum class EMoveSpeed : uint8_t
{
	Invalid = 0,
	Babystep = 1,
	Walk = 2,
	Jog = 3,
	Run = 4,
	Sprint = 5,
	PartnerSkill = 6,
	FollowWalk = 7,
	FollowSprint = 8,
	FollowSwim = 9,
	FollowSprintSwim = 10,
	Interaction = 11,
	EMoveSpeed_MAX = 12
};

// Object: Enum Feature_SP.ESPActionSeqSet_LeaveActionOp
enum class ESPActionSeqSet_LeaveActionOp : uint8_t
{
	None = 0,
	Goto = 1,
	ApplyOffset = 2,
	ESPActionSeqSet_MAX = 3
};

// Object: Enum Feature_SP.ESPActionSeqSet_ActionResultOverrideType
enum class ESPActionSeqSet_ActionResultOverrideType : uint8_t
{
	KeepOrigin = 0,
	DoNothing = 1,
	SuccessOnEnd = 2,
	SuccessOnInterrupted = 3,
	ForceSuccess = 4,
	FailureOnEnd = 5,
	FailureOnInterrupted = 6,
	ForceFailure = 7,
	ForceInterrupted = 8,
	ESPActionSeqSet_MAX = 9
};

// Object: Enum Feature_SP.ESPLevelSignalValueType
enum class ESPLevelSignalValueType : uint8_t
{
	Invalid = 0,
	StringType = 1,
	NumericType = 2,
	BooleanType = 3,
	ESPLevelSignalValueType_MAX = 4
};

// Object: Enum Feature_SP.ESPCompareType
enum class ESPCompareType : uint8_t
{
	Equal = 0,
	Less = 1,
	LessEqual = 2,
	Greater = 3,
	GreaterEqual = 4,
	NotEqual = 5,
	ESPCompareType_MAX = 6
};

// Object: Enum Feature_SP.EDebugInfoType
enum class EDebugInfoType : uint8_t
{
	DebugInfo_None = 0,
	DebugInfo_AI = 1,
	DebugInfo_AI_Confrontational = 2,
	DebugInfo_AI_Swimming = 3,
	DebugInfo_AI_Flyinging = 4,
	DebugInfo_AI_SpecialPerformance = 5,
	DebugInfo_AI_NavAreaConfig = 6,
	DebugInfo_AI_KeepFromOrPathFollow = 7,
	DebugInfo_AI_SOC = 8,
	DebugInfo_AI_SOC_ENABLE_LOG = 9,
	DebugInfo_MAX = 10
};

// Object: Enum Feature_SP.EAssetUsageProbability
enum class EAssetUsageProbability : uint8_t
{
	LikelyToBeUsed = 0,
	MustToBeUsed = 1,
	LowProbabilityToBeUsed = 2,
	WillNotBeUsed = 3,
	MAX = 4
};

// Object: Enum Feature_SP.EAssetUsageGroup
enum class EAssetUsageGroup : uint8_t
{
	Gameplay = 0,
	InBattle = 1,
	Soc = 2,
	MAX = 3
};

// Object: Enum Feature_SP.ESPAssetReleasePolicy
enum class ESPAssetReleasePolicy : uint8_t
{
	Time = 0,
	Manual = 1,
	AutoLRU = 2,
	Never = 3,
	ESPAssetReleasePolicy_MAX = 4
};

// Object: Enum Feature_SP.ESPAsyncLoadFeature
enum class ESPAsyncLoadFeature : uint8_t
{
	None = 0,
	General = 1,
	ProjectileImpactParticle = 2,
	Decal = 4,
	Ability = 8,
	Feature_PlaceHolder2 = 16,
	Feature_PlaceHolder3 = 32,
	Feature_PlaceHolder4 = 64,
	Feature_PlaceHolder5 = 128,
	ESPAsyncLoadFeature_MAX = 129
};

// Object: Enum Feature_SP.EDecorationAttachRule
enum class EDecorationAttachRule : uint8_t
{
	StaticMesh = 0,
	SkeletalMesh = 1,
	EDecorationAttachRule_MAX = 2
};

// Object: Enum Feature_SP.EMeshAttachRule
enum class EMeshAttachRule : uint8_t
{
	AttachStaticMesh = 0,
	AttachSkeletalMesh = 1,
	EMeshAttachRule_MAX = 2
};

// Object: Enum Feature_SP.CalculationFormulaType
enum class ECalculationFormulaType : uint8_t
{
	None = 0,
	OneSubtractionOneSubtractionMultiplier = 1,
	OneAddMultiplier = 2,
	OneSubtractionMultiplier = 3,
	CalculationFormulaType_MAX = 4
};

// Object: Enum Feature_SP.StarPAttrOPType
enum class EStarPAttrOPType : uint8_t
{
	SPAT_OP_None = 0,
	SPAT_OP_Base = 1,
	SPAT_OP_Percent = 2,
	SPAT_OP_Fix = 3,
	SPAT_OP_BaseFix = 4,
	SPAT_OP_BasePercent = 5,
	SPAT_OP_MAX = 6
};

// Object: Enum Feature_SP.SPPetBossType
enum class ESPPetBossType : uint8_t
{
	SP_BOSS_INVALID = 0,
	SP_BOSS_NONE = 1,
	SP_BOSS_GENERAL = 2,
	SP_BOSS_MAX = 3
};

// Object: Enum Feature_SP.ESPOrnamentType
enum class ESPOrnamentType : uint8_t
{
	None = 0,
	Face = 1,
	Head = 2,
	Back = 3,
	HandWear = 4,
	MAX = 5
};

// Object: Enum Feature_SP.ESPOrnamentModeType
enum class ESPOrnamentModeType : uint8_t
{
	None = 0,
	SM = 1,
	SK = 2,
	MAX = 3
};

// Object: Enum Feature_SP.ESPSegmentJumpMode
enum class ESPSegmentJumpMode : uint8_t
{
	Immediately = 0,
	AtEnd = 1,
	ESPSegmentJumpMode_MAX = 2
};

// Object: Enum Feature_SP.EBuildingState
enum class EBuildingState : uint8_t
{
	Unknown = 0,
	Preview = 1,
	PreviewInValid = 2,
	DoBuild = 3,
	BuildComplete = 4,
	PreviewRemove = 5,
	Max = 6
};

// Object: Enum Feature_SP.ESPBuildingSnapType
enum class ESPBuildingSnapType : uint8_t
{
	Point = 1,
	Space = 2,
	ESPBuildingSnapType_MAX = 3
};

// Object: Enum Feature_SP.ESPCaptureBallState
enum class ESPCaptureBallState : uint8_t
{
	None = 0,
	Start = 1,
	Loop = 2,
	Fire = 3,
	Cancel = 4,
	End = 5,
	ESPCaptureBallState_MAX = 6
};

// Object: Enum Feature_SP.ESPCaptureHitType
enum class ESPCaptureHitType : uint8_t
{
	CaptureBallMissed = 0,
	CaptureBallPassThrough = 1,
	CaptureBallRebound = 2,
	TriggerCaptureEvent = 3,
	ESPCaptureHitType_MAX = 4
};

// Object: Enum Feature_SP.ESPCaptureBeginEventType
enum class ESPCaptureBeginEventType : uint16_t
{
	TriggerCapture = 0,
	AllyOrSetNotAbleToCapture = 1,
	NotAllowedToCaptureByConfig = 2,
	TargetDead = 3,
	CaptureValueTooLow = 4,
	FailedToPassCaptureBallTest = 5,
	FailedToPassHPTest = 6,
	FailedToPassDyingTest = 7,
	FailedToPassNotCapturedBeforeTest = 8,
	FailedToPassCaptureBallLevelTest = 9,
	AncientBossByOthersBeast = 10,
	NotAllowedByOperationTeamActivity = 11,
	FailedToPassLevelTest = 12,
	FailedToPassCampureTimes = 13,
	Exception = 255,
	ESPCaptureBeginEventType_MAX = 256
};

// Object: Enum Feature_SP.ESPChangeRateTarget
enum class ESPChangeRateTarget : uint8_t
{
	Ability = 0,
	ESPChangeRateTarget_MAX = 1
};

// Object: Enum Feature_SP.ESPMovementEndState
enum class ESPMovementEndState : uint8_t
{
	NormalEnd = 0,
	InterruptedEnd = 1,
	ESPMovementEndState_MAX = 2
};

// Object: Enum Feature_SP.EDyingStage
enum class EDyingStage : uint8_t
{
	DownToGround = 0,
	LyingOnGround = 1,
	EDyingStage_MAX = 2
};

// Object: Enum Feature_SP.ESPAvatarAsyncLoadState
enum class ESPAvatarAsyncLoadState : uint8_t
{
	Invalid = 0,
	Loading = 1,
	Cancelled = 2,
	Error = 3,
	Finished = 4,
	ESPAvatarAsyncLoadState_MAX = 5
};

// Object: Enum Feature_SP.ESPAvatarMasterSlaveContextState
enum class ESPAvatarMasterSlaveContextState : uint8_t
{
	Created = 0,
	AsyncLoadingResource = 1,
	Finished = 2,
	ESPAvatarMasterSlaveContextState_MAX = 3
};

// Object: Enum Feature_SP.ESPHitDirection
enum class ESPHitDirection : uint8_t
{
	Default = 0,
	Forward = 1,
	Left = 2,
	Back = 3,
	Right = 4,
	ESPHitDirection_MAX = 5
};

// Object: Enum Feature_SP.ESPLoadedHitAnimCacheType
enum class ESPLoadedHitAnimCacheType : uint8_t
{
	ELoadedAnimCacheType_HitIdlePose = 1,
	ELoadedAnimCacheType_HitBlendSpace = 2,
	ELoadedAnimCacheType_StunLoop = 3
};

// Object: Enum Feature_SP.ESPHitType
enum class ESPHitType : uint8_t
{
	HitLight = 0,
	HitBack = 1,
	HitFly = 2,
	ESPHitType_MAX = 3
};

// Object: Enum Feature_SP.EClimbAnimState
enum class EClimbAnimState : uint8_t
{
	Invalid = 0,
	Monkey = 1,
	EClimbAnimState_MAX = 2
};

// Object: Enum Feature_SP.ESPClimbTimerType
enum class ESPClimbTimerType : uint8_t
{
	Fall = 0,
	MantleUp = 1,
	MantleForward = 2,
	StartClimbingGround = 3,
	ESPClimbTimerType_MAX = 4
};

// Object: Enum Feature_SP.ESPClimbLocomotionState
enum class ESPClimbLocomotionState : uint8_t
{
	None = 0,
	Ground = 1,
	Falling = 2,
	Climbing = 3,
	Mantle = 4,
	ESPClimbLocomotionState_MAX = 5
};

// Object: Enum Feature_SP.ESPCollisionActorType
enum class ESPCollisionActorType : uint8_t
{
	DamageActor = 0,
	Obstacle = 1,
	ESPCollisionActorType_MAX = 2
};

// Object: Enum Feature_SP.ESPComboAttackType
enum class ESPComboAttackType : uint8_t
{
	None = 0,
	Attack = 1,
	ChargeAttack = 2,
	ESPComboAttackType_MAX = 3
};

// Object: Enum Feature_SP.ESPDataTableReadMode
enum class ESPDataTableReadMode : uint8_t
{
	SeekLua = 0,
	Row = 1,
	All = 2,
	ESPDataTableReadMode_MAX = 3
};

// Object: Enum Feature_SP.ESPDataTableType
enum class ESPDataTableType : uint8_t
{
	None = 0,
	Number = 1,
	String = 2,
	Bool = 3,
	Table = 4,
	ArrayNumber = 5,
	ArrayString = 6,
	ArrayBool = 7,
	ArrayTable = 8,
	Int64 = 9,
	ArrayInt64 = 10,
	ESPDataTableType_MAX = 11
};

// Object: Enum Feature_SP.SPGameAnimModule_SwimType
enum class ESPGameAnimModule_SwimType : uint8_t
{
	StartSwim = 0,
	SwimLoop = 1,
	EndSwim = 2,
	SPGameAnimModule_MAX = 3
};

// Object: Enum Feature_SP.SPGameAnimModule_RideType
enum class ESPGameAnimModule_RideType : uint8_t
{
	StartRide = 0,
	RideLoop = 1,
	EndRide = 2,
	SPGameAnimModule_MAX = 3
};

// Object: Enum Feature_SP.SPGameAnimModule_ClimbType
enum class ESPGameAnimModule_ClimbType : uint8_t
{
	StartClimb = 0,
	ClimbLoop = 1,
	EndClimb = 2,
	SPGameAnimModule_MAX = 3
};

// Object: Enum Feature_SP.AttributeChangeReason
enum class EAttributeChangeReason : uint8_t
{
	None = 0,
	SetAttribute = 1,
	AddAttribute = 2,
	AttributeChangeReason_MAX = 3
};

// Object: Enum Feature_SP.AttributeUnAcceptableReason
enum class EAttributeUnAcceptableReason : uint8_t
{
	OverMaxValue = 0,
	OverLimitValue = 1,
	BelowLowerLimitValue = 2,
	None = 63,
	AttributeUnAcceptableReason_MAX = 64
};

// Object: Enum Feature_SP.ESPGameBuildingInstCompOutReasonMasks
enum class ESPGameBuildingInstCompOutReasonMasks : uint8_t
{
	Normal = 0,
	Replace = 1,
	ActorEndPlay = 2,
	ESPGameBuildingInstCompOutReasonMasks_MAX = 3
};

// Object: Enum Feature_SP.ESPGameBuildingInstCompInReasonMasks
enum class ESPGameBuildingInstCompInReasonMasks : uint8_t
{
	Normal = 0,
	Replace = 1,
	ESPGameBuildingInstCompInReasonMasks_MAX = 2
};

// Object: Enum Feature_SP.ESPGameBuildingSceneObjectUserType
enum class ESPGameBuildingSceneObjectUserType : uint8_t
{
	SceneObjectUserType_NONE = 0,
	SceneObjectUserType_Actor = 1,
	SceneObjectUserType_BlockGroupComp = 2,
	SceneObjectUserType_MAX = 3
};

// Object: Enum Feature_SP.ESPAnimModule_AnimUpperBodyActionState
enum class ESPAnimModule_AnimUpperBodyActionState : uint8_t
{
	None = 0,
	Aiming = 1,
	LeashedPet = 2,
	ESPAnimModule_MAX = 3
};

// Object: Enum Feature_SP.ESPInputCommandType
enum class ESPInputCommandType : uint8_t
{
	Invalid = 0,
	Skill = 1,
	FriendSkill = 2,
	ESPInputCommandType_MAX = 3
};

// Object: Enum Feature_SP.ESPWeaponShowHideReason
enum class ESPWeaponShowHideReason : uint8_t
{
	None = 0,
	Equipping = 1,
	UnEquip = 2,
	OwnerStateChange = 3,
	Ability = 4,
	OwnerDeath = 5,
	StoryLine = 6,
	AnimNotify = 7,
	ESPWeaponShowHideReason_MAX = 8
};

// Object: Enum Feature_SP.ESPAimAssistFilterType
enum class ESPAimAssistFilterType : uint8_t
{
	Angle = 0,
	Distance = 1,
	Health = 2,
	ESPAimAssistFilterType_MAX = 3
};

// Object: Enum Feature_SP.ESPLookAtAnimMode
enum class ESPLookAtAnimMode : uint8_t
{
	Disabled = 0,
	LegacyVersion = 1,
	LookAtControlVersion = 2,
	ESPLookAtAnimMode_MAX = 3
};

// Object: Enum Feature_SP.ESPCaptureBallCostPolicy
enum class ESPCaptureBallCostPolicy : uint8_t
{
	Spawn = 0,
	TriggerCapture = 1,
	ESPCaptureBallCostPolicy_MAX = 2
};

// Object: Enum Feature_SP.ESPSwapPetPosPolicy
enum class ESPSwapPetPosPolicy : uint8_t
{
	NotSwap = 0,
	Swap = 1,
	Break = 2,
	SwapButNotBreak = 3,
	SwapAndBreak = 4,
	ESPSwapPetPosPolicy_MAX = 5
};

// Object: Enum Feature_SP.ESPPetFocusMode
enum class ESPPetFocusMode : uint8_t
{
	None = 0,
	NearestPet = 1,
	UnleashedPet = 2,
	ESPPetFocusMode_MAX = 3
};

// Object: Enum Feature_SP.ESPGameEnvType
enum class ESPGameEnvType : uint8_t
{
	SPGame = 0,
	Dungeon = 1,
	PVP = 2,
	Group = 3,
	ESPGameEnvType_MAX = 4
};

// Object: Enum Feature_SP.ECharAbnormalJudgement
enum class ECharAbnormalJudgement : uint8_t
{
	CAJ_Open = 0,
	CAJ_Block = 1,
	CAJ_MAX = 2
};

// Object: Enum Feature_SP.ECharStateJudgement
enum class ECharStateJudgement : uint8_t
{
	CSJ_Open = 0,
	CSJ_Block = 1,
	CSJ_MAX = 2
};

// Object: Enum Feature_SP.ESPMonsterAttributeType
enum class ESPMonsterAttributeType : uint8_t
{
	None = 0,
	SP_PET_ATTRIBUTE_TYPE_NORMAL = 1,
	SP_PET_ATTRIBUTE_TYPE_ELITE = 2,
	SP_PET_ATTRIBUTE_TYPE_BOSS = 3,
	SP_PET_ATTRIBUTE_TYPE_InVisible = 4,
	ESPMonsterAttributeType_MAX = 5
};

// Object: Enum Feature_SP.ESPStageState
enum class ESPStageState : uint8_t
{
	None = 0,
	SP_Monster_Default = 1,
	SP_Monster_TenacityEnter = 2,
	SP_Monster_TenacityBreak = 3,
	SP_Monster_TenacityRecover = 4,
	SP_Monster_TenacityParalyzed = 5,
	SP_Monster_TenacityFierce = 6,
	End = 7,
	ESPStageState_MAX = 8
};

// Object: Enum Feature_SP.ESPStateListenType
enum class ESPStateListenType : uint8_t
{
	SP_Monster_Dying = 0,
	SP_Monster_Battle = 1,
	SP_Monster_MAX = 2
};

// Object: Enum Feature_SP.ESPGamePlayDebugShape
enum class ESPGamePlayDebugShape : uint8_t
{
	Point = 0,
	Line = 1,
	Box = 2,
	Cylinder = 3,
	Polygon = 4,
	ESPGamePlayDebugShape_MAX = 5
};

// Object: Enum Feature_SP.ELookAtType
enum class ELookAtType : uint8_t
{
	Disable = 0,
	LookAtFromConfront = 1,
	LookAtFromAble = 2,
	LookAtFromFondle = 3,
	ELookAtType_MAX = 4
};

// Object: Enum Feature_SP.ESPCheckDmgValidation
enum class ESPCheckDmgValidation : uint8_t
{
	Success = 0,
	DamageConditionCheckFailed = 1,
	InvalidReasonStart = 10,
	SPActorInterfaceInvalid = 11,
	HitResultCheckFailed = 12,
	SkillIdCheckFailed = 13,
	DamageIdCheckFailed = 14,
	ESPCheckDmgValidation_MAX = 15
};

// Object: Enum Feature_SP.ESPTraceShape
enum class ESPTraceShape : uint8_t
{
	None = 0,
	Sphere = 1,
	Box = 2,
	Capsule = 3,
	ESPTraceShape_MAX = 4
};

// Object: Enum Feature_SP.ESPTraceAmountType
enum class ESPTraceAmountType : uint8_t
{
	None = 0,
	SingleTrace = 1,
	MultiTrace = 2,
	ESPTraceAmountType_MAX = 3
};

// Object: Enum Feature_SP.ESPProjectileTerminateReason
enum class ESPProjectileTerminateReason : uint8_t
{
	None = 0,
	Manual = 1,
	TraceBlocked = 2,
	TimeOut = 3,
	ReachMaxReflectTime = 4,
	ReachTarget = 5,
	ReachOwner = 6,
	ReachCurveEnd = 7,
	AbleEnd = 8,
	TargetScopeInvalid = 9,
	ReachFlyTime = 10,
	DetachFromDS = 11,
	Invalid = 12,
	ESPProjectileTerminateReason_MAX = 13
};

// Object: Enum Feature_SP.ESPProjectileInstigatorType
enum class ESPProjectileInstigatorType : uint8_t
{
	None = 0,
	WildBoss = 1,
	Player = 2,
	Monster = 3,
	Invalid = 4,
	ESPProjectileInstigatorType_MAX = 5
};

// Object: Enum Feature_SP.ESPDeadReason
enum class ESPDeadReason : uint8_t
{
	None = 1,
	Cold = 2,
	Hot = 3,
	Water = 4,
	Hunger = 5,
	Fall = 6,
	ESPDeadReason_MAX = 7
};

// Object: Enum Feature_SP.ESPDamageType
enum class ESPDamageType : uint8_t
{
	Normal = 0,
	GroundSlight = 1,
	GroundHeavy = 2,
	InAirSlight = 3,
	InAirHeavy = 4,
	UnderWaterSlight = 5,
	UnderWaterHeavy = 6,
	Extra = 7,
	ESPDamageType_MAX = 8
};

// Object: Enum Feature_SP.EHiddenType
enum class EHiddenType : uint8_t
{
	HideAll = 0,
	HidePart = 1,
	Recover = 2,
	EHiddenType_MAX = 3
};

// Object: Enum Feature_SP.ESPAIEventType
enum class ESPAIEventType : uint8_t
{
	Invalid = 0,
	PlayerInteraction = 1,
	PlayerOrder = 2,
	SyncAnimByHost = 3,
	DeathEvent = 4,
	AttackNotify = 5,
	AIFollowing = 6,
	HurtEvent = 7,
	CaptureEvent = 8,
	AIAttackEvent = 9,
	AIMoveToEvent = 10,
	AutonomousInteraction = 11,
	SpreadThreat = 12,
	ESPAIEventType_MAX = 13
};

// Object: Enum Feature_SP.ESPFriendSkillResumAIReason
enum class ESPFriendSkillResumAIReason : uint8_t
{
	Unknown = 0,
	Dismount = 1,
	ESPFriendSkillResumAIReason_MAX = 2
};

// Object: Enum Feature_SP.ESPFriendSkillStopAIReason
enum class ESPFriendSkillStopAIReason : uint8_t
{
	Unknown = 0,
	Mount = 1,
	ESPFriendSkillStopAIReason_MAX = 2
};

// Object: Enum Feature_SP.ESPCostEnergyType
enum class ESPCostEnergyType : uint8_t
{
	Invalid = 0,
	HP = 1,
	Stamina = 2,
	SkillEnergy = 3,
	FriendSkillEnergy = 4,
	Barrier = 5,
	Custom = 6,
	ESPCostEnergyType_MAX = 7
};

// Object: Enum Feature_SP.ESPUIButtonState
enum class ESPUIButtonState : uint8_t
{
	None = 0,
	Disabled = 1,
	Hidden = 2,
	HighLight = 3,
	ESPUIButtonState_MAX = 4
};

// Object: Enum Feature_SP.ESPAimingType
enum class ESPAimingType : uint8_t
{
	Capture = 0,
	RidingCapture = 1,
	ESPAimingType_MAX = 2
};

// Object: Enum Feature_SP.ESPAbilityTaskTargetPosType
enum class ESPAbilityTaskTargetPosType : uint8_t
{
	Default = 0,
	WeakPoint = 1,
	Bone = 2,
	ESPAbilityTaskTargetPosType_MAX = 3
};

// Object: Enum Feature_SP.ESPAirWallOverlapType
enum class ESPAirWallOverlapType : uint8_t
{
	Player = 0,
	Pet = 1,
	Monster = 2,
	ESPAirWallOverlapType_MAX = 3
};

// Object: Enum Feature_SP.EPetInteractResult
enum class EPetInteractResult : uint8_t
{
	Success = 0,
	Failed = 1,
	TimeOut = 2,
	EPetInteractResult_MAX = 3
};

// Object: Enum Feature_SP.ESPUnleashQueryResult
enum class ESPUnleashQueryResult : uint8_t
{
	InstanceIdInvalid = 0,
	InstanceIdSame = 1,
	PetInDyingState = 2,
	UnableToUnleashByPetState = 3,
	UnableToUnleashByPlayerState = 4,
	QueryFailed = 5,
	NoPetOnField = 6,
	InstanceIdDifferent = 7,
	ESPUnleashQueryResult_MAX = 8
};

// Object: Enum Feature_SP.ESPActorInWorldStateMask
enum class ESPActorInWorldStateMask : uint8_t
{
	LogicallyHidden = 0,
	PhysicallyHidden = 1,
	VisuallyHidden = 2,
	IgnoreGravity = 3,
	Max = 4
};

// Object: Enum Feature_SP.ESPMonsterFlags
enum class ESPMonsterFlags : uint64_t
{
	None = 0,
	Untargetable = 1,
	Max = 18446744071562067968
};

// Object: Enum Feature_SP.EEmoteState
enum class EEmoteState : uint8_t
{
	Montage = 0,
	AI = 1,
	State = 2,
	None = 3,
	EEmoteState_MAX = 4
};

// Object: Enum Feature_SP.ESPMonsterCoreInfoType
enum class ESPMonsterCoreInfoType : uint8_t
{
	CoreInfoTypeBegin = 0,
	MonsterUID = 1,
	MonsterID = 2,
	MonsterSpawnType = 3,
	CoreInfoTypeEnd = 4,
	ESPMonsterCoreInfoType_MAX = 5
};

// Object: Enum Feature_SP.ESPGameplayMessageMatch
enum class ESPGameplayMessageMatch : uint8_t
{
	ExactMatch = 0,
	PartialMatch = 1,
	ESPGameplayMessageMatch_MAX = 2
};

// Object: Enum Feature_SP.ESPTileActorFilter
enum class ESPTileActorFilter : uint8_t
{
	Monster = 0,
	Player = 1,
	None = 2,
	ESPTileActorFilter_MAX = 3
};

// Object: Enum Feature_SP.ESPHitActorType
enum class ESPHitActorType : uint8_t
{
	Invalid = 0,
	Player = 1,
	Monster = 2,
	Wood = 4,
	Rock = 5,
	ESPHitActorType_MAX = 6
};

// Object: Enum Feature_SP.EHitCoverType
enum class EHitCoverType : uint8_t
{
	OnlySelf = 0,
	AllowOthers = 1,
	EHitCoverType_MAX = 2
};

// Object: Enum Feature_SP.ESPSegmentTaskPhase
enum class ESPSegmentTaskPhase : uint8_t
{
	None = 0,
	Checking = 1,
	Passed = 2,
	Failed = 3,
	ESPSegmentTaskPhase_MAX = 4
};

// Object: Enum Feature_SP.ESPInputBranchSegmentJumpMode
enum class ESPInputBranchSegmentJumpMode : uint8_t
{
	Immediately = 0,
	AtEnd = 1,
	Custom = 2,
	Force = 3,
	ESPInputBranchSegmentJumpMode_MAX = 4
};

// Object: Enum Feature_SP.ESPSegmentInputMode
enum class ESPSegmentInputMode : uint8_t
{
	Pressed = 0,
	Released = 1,
	Clicked = 2,
	Hold = 3,
	UpToDown = 4,
	ESPSegmentInputMode_MAX = 5
};

// Object: Enum Feature_SP.ESPInputBranchSegmentCheckMode
enum class ESPInputBranchSegmentCheckMode : uint8_t
{
	Segment = 0,
	Combo = 1,
	Check = 2,
	ESPInputBranchSegmentCheckMode_MAX = 3
};

// Object: Enum Feature_SP.EInputControlType
enum class EInputControlType : uint8_t
{
	None = 0,
	DisableInput = 1,
	ListenInput = 2,
	InputInterruptActionState = 3,
	EInputControlType_MAX = 4
};

// Object: Enum Feature_SP.ESPTouchType
enum class ESPTouchType : uint8_t
{
	Started = 0,
	Moved = 1,
	Released = 2,
	ESPTouchType_MAX = 3
};

// Object: Enum Feature_SP.EJamDetectObject
enum class EJamDetectObject : uint8_t
{
	Box = 0,
	LandScape = 1,
	All = 2,
	EJamDetectObject_MAX = 3
};

// Object: Enum Feature_SP.EJamDetectCategory
enum class EJamDetectCategory : uint8_t
{
	Z = 0,
	XY = 1,
	Hollow = 2,
	All = 3,
	EJamDetectCategory_MAX = 4
};

// Object: Enum Feature_SP.ESPJumpLocationToAttachPoint
enum class ESPJumpLocationToAttachPoint : uint8_t
{
	Undefined = 0,
	Center = 1,
	Front = 2,
	Back = 3,
	Left = 4,
	Right = 5,
	ESPJumpLocationToAttachPoint_MAX = 6
};

// Object: Enum Feature_SP.ELevelUIStateType
enum class ELevelUIStateType : uint8_t
{
	Default = 0,
	OW_SingleMiniGame = 1,
	OW_MultiGame = 2,
	ELevelUIStateType_MAX = 3
};

// Object: Enum Feature_SP.ESP_BlackSceneType
enum class ESP_BlackSceneType : uint8_t
{
	BLACK = 0,
	LOADING_BAR = 1,
	ESP_MAX = 2
};

// Object: Enum Feature_SP.ELevelTimerUIType
enum class ELevelTimerUIType : uint8_t
{
	Normal = 1,
	CatchBoss = 2,
	Normal_Top = 3,
	World_Boss = 4,
	Operation_Team = 5,
	ELevelTimerUIType_MAX = 6
};

// Object: Enum Feature_SP.ESPLevelCutSceneResType
enum class ESPLevelCutSceneResType : uint8_t
{
	PandoraVideo = 0,
	LevelSequence = 1,
	ESPLevelCutSceneResType_MAX = 2
};

// Object: Enum Feature_SP.ESPPlayerReadyUIType
enum class ESPPlayerReadyUIType : uint8_t
{
	MULTIPVE = 0,
	ESPPlayerReadyUIType_MAX = 1
};

// Object: Enum Feature_SP.ESPEncounterUIPromptType
enum class ESPEncounterUIPromptType : uint8_t
{
	CENTERTOP = 0,
	Discover_Common = 1,
	Challenge_Begin = 2,
	Boss_Catch = 3,
	Tip_Common = 4,
	Battle_End = 5,
	After_Battle_End = 6,
	ESPEncounterUIPromptType_MAX = 7
};

// Object: Enum Feature_SP.ESPLevelAIPerceptionType
enum class ESPLevelAIPerceptionType : uint8_t
{
	PLAYER = 0,
	ESPLevelAIPerceptionType_MAX = 1
};

// Object: Enum Feature_SP.ESPRegionEnterTargetType
enum class ESPRegionEnterTargetType : uint8_t
{
	PLAYER = 0,
	ESPRegionEnterTargetType_MAX = 1
};

// Object: Enum Feature_SP.ESPLevelTargetType
enum class ESPLevelTargetType : uint8_t
{
	AIGROUP = 0,
	GPOGROUP = 1,
	VIRTUALPOINTGROUP = 2,
	AIRWALLGROUP = 3,
	ESPLevelTargetType_MAX = 4
};

// Object: Enum Feature_SP.ESPExtraItemGroupType
enum class ESPExtraItemGroupType : uint8_t
{
	AIGROUP = 0,
	ESPExtraItemGroupType_MAX = 1
};

// Object: Enum Feature_SP.ESPItemDepositTargetType
enum class ESPItemDepositTargetType : uint8_t
{
	GPOGROUP = 0,
	AIGROUP_PLAYERGROUP = 1,
	ESPItemDepositTargetType_MAX = 2
};

// Object: Enum Feature_SP.ESPLevelProgressType
enum class ESPLevelProgressType : uint8_t
{
	NORMAL = 0,
	NUMBER = 1,
	PERCENTAGE = 2,
	ESPLevelProgressType_MAX = 3
};

// Object: Enum Feature_SP.ELevelTargetUIType
enum class ELevelTargetUIType : uint8_t
{
	Dungeon = 0,
	OW_SingleMiniGame = 1,
	OW_MultiGame = 2,
	ELevelTargetUIType_MAX = 3
};

// Object: Enum Feature_SP.ESPFactionType
enum class ESPFactionType : uint8_t
{
	UNKNOWN = 0,
	PLAYER = 1,
	PASSIVEMONSTER = 2,
	AGGRESSIVEMONSTER = 3,
	FRIENDLYPAWN = 4,
	NEUTRALPAWN = 5,
	HOSTILEPAWN = 6,
	GAMEPLAY01 = 7,
	GAMEPLAY02 = 8,
	GAMEPLAY03 = 9,
	GAMEPLAY04 = 10,
	ESPFactionType_MAX = 11
};

// Object: Enum Feature_SP.SPAINumOperator
enum class ESPAINumOperator : uint8_t
{
	EQUAL = 0,
	NOTEQUAL = 1,
	GREATER = 2,
	LESS = 3,
	SPAINumOperator_MAX = 4
};

// Object: Enum Feature_SP.ELogicResponseType
enum class ELogicResponseType : uint8_t
{
	Disable = 0,
	Enable = 1,
	ELogicResponseType_MAX = 2
};

// Object: Enum Feature_SP.ESPLevelGpoLogicType
enum class ESPLevelGpoLogicType : uint8_t
{
	BeAttacked = 0,
	Interaction = 1,
	Life = 2,
	Move = 3,
	Range = 4,
	TargetIndicator = 5,
	View = 6,
	ESPLevelGpoLogicType_MAX = 7
};

// Object: Enum Feature_SP.ESPLevelLayerObjectType
enum class ESPLevelLayerObjectType : uint8_t
{
	None = 0,
	AICreator = 1,
	AISTP = 2,
	AirWall = 3,
	Patrol = 4,
	Zone = 5,
	GameObj = 6,
	VirtualPoint = 7,
	ESPLevelLayerObjectType_MAX = 8
};

// Object: Enum Feature_SP.ESPLevelLayerTriggerType
enum class ESPLevelLayerTriggerType : uint8_t
{
	Box = 0,
	Sphere = 1,
	Polygon = 2,
	ESPLevelLayerTriggerType_MAX = 3
};

// Object: Enum Feature_SP.ESPSequenceLogType
enum class ESPSequenceLogType : uint8_t
{
	Log = 0,
	Warning = 1,
	Error = 2,
	ESPSequenceLogType_MAX = 3
};

// Object: Enum Feature_SP.ESPCreateLevelSequencePlayerType
enum class ESPCreateLevelSequencePlayerType : uint8_t
{
	NotInit = 0,
	Standard = 1,
	DirectFinish = 2,
	DSError = 3,
	ESPCreateLevelSequencePlayerType_MAX = 4
};

// Object: Enum Feature_SP.ESPLevelSequencePlayState
enum class ESPLevelSequencePlayState : uint8_t
{
	NeverPlayed = 0,
	Playing = 1,
	Finished = 2,
	ESPLevelSequencePlayState_MAX = 3
};

// Object: Enum Feature_SP.ECancelStarPalOutlineReason
enum class ECancelStarPalOutlineReason : uint8_t
{
	None = 0,
	HitTransparent = 1,
	ECancelStarPalOutlineReason_MAX = 2
};

// Object: Enum Feature_SP.ERunningMaterialZWriteMode
enum class ERunningMaterialZWriteMode : uint8_t
{
	AllWithZWrite = 0,
	AnyWithoutZWrite = 1,
	NeedCheck = 2,
	ERunningMaterialZWriteMode_MAX = 3
};

// Object: Enum Feature_SP.ParameterInheritanceValidFlag
enum class EParameterInheritanceValidFlag : uint8_t
{
	NotChecked = 0,
	Valid = 1,
	Invalid = 2,
	ParameterInheritanceValidFlag_MAX = 3
};

// Object: Enum Feature_SP.EMaterialSetItemSlotFilterType
enum class EMaterialSetItemSlotFilterType : uint8_t
{
	Invalid = 0,
	BlackListed = 1,
	WhiteListed = 2,
	EMaterialSetItemSlotFilterType_MAX = 3
};

// Object: Enum Feature_SP.ESPRunningFatThinMode
enum class ESPRunningFatThinMode : uint8_t
{
	Undefined = 0,
	Fat = 1,
	Thin = 2,
	Transforming = 3,
	ESPRunningFatThinMode_MAX = 4
};

// Object: Enum Feature_SP.ESPRandomRideFlyType
enum class ESPRandomRideFlyType : uint8_t
{
	CompletelyRandom = 0,
	ConditionRandom = 1,
	ESPRandomRideFlyType_MAX = 2
};

// Object: Enum Feature_SP.EMonsterPart
enum class EMonsterPart : uint8_t
{
	HEAD = 0,
	EMonsterPart_MAX = 1
};

// Object: Enum Feature_SP.EParticleEffectType
enum class EParticleEffectType : uint8_t
{
	TemporaryParticleEffect = 0,
	PermanentParticleEffect = 1,
	EParticleEffectType_MAX = 2
};

// Object: Enum Feature_SP.EPSCAttachComp
enum class EPSCAttachComp : uint8_t
{
	Root = 0,
	Mesh = 1,
	EPSCAttachComp_MAX = 2
};

// Object: Enum Feature_SP.ESPAttachMeshHideReason
enum class ESPAttachMeshHideReason : uint8_t
{
	AttachedParent = 0,
	Animation = 1,
	ESPAttachMeshHideReason_MAX = 2
};

// Object: Enum Feature_SP.ESPAttachMeshVisibilityPolicy
enum class ESPAttachMeshVisibilityPolicy : uint8_t
{
	FollowParent = 0,
	InternalControl = 1,
	ESPAttachMeshVisibilityPolicy_MAX = 2
};

// Object: Enum Feature_SP.ESPAttachMeshType
enum class ESPAttachMeshType : uint8_t
{
	Static = 0,
	Skeletal = 1,
	ESPAttachMeshType_MAX = 2
};

// Object: Enum Feature_SP.EMonsterStorageType
enum class EMonsterStorageType : uint8_t
{
	ENone = 0,
	EInTeam = 1,
	EInWareHouse = 2,
	EOtherRepToAll = 3,
	EMonsterStorageType_MAX = 4
};

// Object: Enum Feature_SP.ESPMorphStateCode
enum class ESPMorphStateCode : uint8_t
{
	None = 0,
	Entering = 1,
	Morphing = 2,
	Leaving = 3,
	ESPMorphStateCode_MAX = 4
};

// Object: Enum Feature_SP.ESPMorphStageCode
enum class ESPMorphStageCode : uint8_t
{
	None = 0,
	MorphEnterStarted = 1,
	MorphEnterFinished = 2,
	MorphExitStarted = 11,
	MorphExitFinished = 12,
	ESPMorphStageCode_MAX = 13
};

// Object: Enum Feature_SP.ESPMorphResultCode
enum class ESPMorphResultCode : uint8_t
{
	Successful = 0,
	Failed_StateForbidden = 1,
	Failed_EnergyDontEnough = 2,
	Failed_ConfigNotFound = 3,
	Failed_NotValidOwner = 4,
	Failed_MonsterMissing = 5,
	Failed_AlreadyInMorphing = 6,
	Failed_NotInMorphing = 7,
	Failed_InvalidItemInSlot = 8,
	Failed_DungeonFlyingMorphForbidden = 9,
	Failed_AlreadyInSameState = 10,
	Failed_PendingLock = 11,
	ESPMorphResultCode_MAX = 12
};

// Object: Enum Feature_SP.EAdsorptiveMovementMode
enum class EAdsorptiveMovementMode : uint8_t
{
	None = 0,
	AccelerationMode = 1,
	VelocityMode = 2,
	DistanceRateMode = 3,
	EAdsorptiveMovementMode_MAX = 4
};

// Object: Enum Feature_SP.EMovementProcessorPriority
enum class EMovementProcessorPriority : uint8_t
{
	RunAnimMove = 0,
	CoverAnimMove = 1,
	SkipAnimMove = 2,
	SkipProcessor = 4,
	EMovementProcessorPriority_MAX = 5
};

// Object: Enum Feature_SP.EMovementState
enum class EMovementState : uint8_t
{
	UnInitialized = 0,
	Moving = 1,
	Completed = 2,
	Interrupted = 3,
	Aborted = 4,
	EMovementState_MAX = 5
};

// Object: Enum Feature_SP.ESP_DefaultGraphicQualityLevel
enum class ESP_DefaultGraphicQualityLevel : uint8_t
{
	Default = 0,
	Low = 1,
	Medium = 2,
	High = 3,
	Ultra = 4,
	Count = 5,
	ESP_MAX = 6
};

// Object: Enum Feature_SP.ESPPetInteractionButtonState
enum class ESPPetInteractionButtonState : uint8_t
{
	Visible = 0,
	Hidden = 1,
	ESPPetInteractionButtonState_MAX = 2
};

// Object: Enum Feature_SP.ESPPetInteractionButtonType
enum class ESPPetInteractionButtonType : uint8_t
{
	ButtonTypeBegin = 0,
	InteractInCombatTeam = 1,
	InteractInTerminal = 2,
	LeftLift = 3,
	RightLift = 4,
	RightThrow = 5,
	PanelLift = 6,
	SwapPosition = 7,
	RightRelieve = 8,
	ButtonTypeEnd = 9,
	ESPPetInteractionButtonType_MAX = 10
};

// Object: Enum Feature_SP.EMoeSPInputSensitiveType
enum class EMoeSPInputSensitiveType : uint8_t
{
	Invalid = 0,
	Normal = 1,
	Ridden = 2,
	ThrowBall = 3,
	RangeWeapon = 4,
	MAX_Value = 5,
	EMoeSPInputSensitiveType_MAX = 6
};

// Object: Enum Feature_SP.EStateBlockSource
enum class EStateBlockSource : uint8_t
{
	None = 0,
	MotionState = 1,
	ActionState = 2,
	Falling = 3,
	EStateBlockSource_MAX = 4
};

// Object: Enum Feature_SP.ESPCurrentPerformanceLevel
enum class ESPCurrentPerformanceLevel : uint8_t
{
	Default = 0,
	Extreme = 1,
	Ultra = 2,
	High = 3,
	Medium = 4,
	Low = 5,
	Count = 6,
	ESPCurrentPerformanceLevel_MAX = 7
};

// Object: Enum Feature_SP.ETickGroupPrimedID
enum class ETickGroupPrimedID : uint8_t
{
	Default = 0,
	TickGroupPrimed = 1,
	TickGroupPrimed = 2,
	TickGroupPrimed = 3,
	TickGroupPrimed = 5,
	TickGroupPrimed = 7,
	TickGroupPrimed = 11,
	TickGroupPrimed = 13,
	TickGroupPrimed = 17,
	TickGroupPrimed = 19,
	TickGroupPrimed = 23,
	TickGroupPrimed = 29,
	Count = 30,
	ETickGroupPrimedID_MAX = 31
};

// Object: Enum Feature_SP.ESPTargetSortRule
enum class ESPTargetSortRule : uint8_t
{
	Default = 0,
	DistanceAscend = 1,
	DistanceDescend = 2,
	AbsHPAscend = 3,
	AbsHPDescend = 4,
	RelHPAscend = 5,
	RelHPDescend = 6,
	PetPrioritized = 7,
	PlayerPrioritized = 8,
	ESPTargetSortRule_MAX = 9
};

// Object: Enum Feature_SP.ESPWoundEffect
enum class ESPWoundEffect : uint8_t
{
	None = 0,
	ConditionDetonate = 1,
	DirectDetonate = 2,
	ESPWoundEffect_MAX = 3
};

// Object: Enum Feature_SP.ESPProjectileDamageType
enum class ESPProjectileDamageType : uint8_t
{
	Single = 0,
	AOE = 1,
	ESPProjectileDamageType_MAX = 2
};

// Object: Enum Feature_SP.ESPPVPSideType
enum class ESPPVPSideType : uint8_t
{
	SP_PVP_NONE = 0,
	SP_PVP_SIDE = 1,
	SP_PVP_SIDE = 2,
	SP_PVP_SIDE_INVALID = 3,
	SP_PVP_MAX = 4
};

// Object: Enum Feature_SP.ESPPVPCubicPointFlag
enum class ESPPVPCubicPointFlag : uint8_t
{
	None = 0,
	CubicPoint = 1,
	Vertex = 2,
	Edge = 4,
	ESPPVPCubicPointFlag_MAX = 5
};

// Object: Enum Feature_SP.ESPPVPCubicMode
enum class ESPPVPCubicMode : uint8_t
{
	Rectangle = 0,
	Hexagon = 1,
	ESPPVPCubicMode_MAX = 2
};

// Object: Enum Feature_SP.ESPPVPCubicDrawType
enum class ESPPVPCubicDrawType : uint8_t
{
	NONE = 0,
	CUBIC = 1,
	ACTOR = 2,
	FIND_PATH = 3,
	LOCKED = 4,
	MAX = 5
};

// Object: Enum Feature_SP.ESPPVPGame_DistanceType
enum class ESPPVPGame_DistanceType : uint8_t
{
	WorldDistance = 0,
	CubicDistance = 1,
	PathDistance = 2,
	ESPPVPGame_MAX = 3
};

// Object: Enum Feature_SP.ESPPVPGame_PVPMonsterFightStage
enum class ESPPVPGame_PVPMonsterFightStage : uint8_t
{
	Fight = 0,
	Watch = 1,
	ESPPVPGame_MAX = 2
};

// Object: Enum Feature_SP.ESPPVPGame_FilterType
enum class ESPPVPGame_FilterType : uint8_t
{
	Include = 0,
	Exclude = 1,
	ESPPVPGame_MAX = 2
};

// Object: Enum Feature_SP.ESPPVPGame_RestrainType
enum class ESPPVPGame_RestrainType : uint8_t
{
	Restrain = 0,
	Restrained = 1,
	None = 2,
	ESPPVPGame_MAX = 3
};

// Object: Enum Feature_SP.ESPPVPGame_LockedType
enum class ESPPVPGame_LockedType : uint8_t
{
	Locked = 0,
	Unlocked = 1,
	ESPPVPGame_MAX = 2
};

// Object: Enum Feature_SP.ESPPVPGame_RelationType
enum class ESPPVPGame_RelationType : uint8_t
{
	ALL = 0,
	ENEMY = 1,
	FRIEND = 2,
	ESPPVPGame_MAX = 3
};

// Object: Enum Feature_SP.ESPPVPGame_MonsterStateFlags
enum class ESPPVPGame_MonsterStateFlags : uint8_t
{
	None = 0,
	OTHER = 1,
	OBSERVE = 2,
	PREPARING = 4,
	BATTLE = 8,
	DEATH = 16,
	SETTLEMENT = 32,
	ESPPVPGame_MAX = 33
};

// Object: Enum Feature_SP.ESPPVPGame_StageFlags
enum class ESPPVPGame_StageFlags : uint8_t
{
	NONE = 0,
	INVALID = 1,
	INIT = 2,
	PREPARE = 4,
	READYGO = 8,
	FIGHTING = 16,
	RESET = 32,
	SETTLEMENT = 64,
	ESPPVPGame_MAX = 65
};

// Object: Enum Feature_SP.ESPPVPGame_CompareType
enum class ESPPVPGame_CompareType : uint8_t
{
	Greater = 0,
	GreaterOrEqual = 1,
	Equal = 2,
	LessOrEqual = 3,
	Less = 4,
	ESPPVPGame_MAX = 5
};

// Object: Enum Feature_SP.ESPPVPGame_JobTypeFlags
enum class ESPPVPGame_JobTypeFlags : uint8_t
{
	None = 0,
	INVALID = 1,
	WARRIOR = 2,
	MAGE = 4,
	SUPPORT = 8,
	THERAPIST = 16,
	TANK = 32,
	ASSASSIN = 64,
	ESPPVPGame_MAX = 65
};

// Object: Enum Feature_SP.ESPPVPGame_OrderingType
enum class ESPPVPGame_OrderingType : uint8_t
{
	Ascending = 0,
	Descending = 1,
	ESPPVPGame_MAX = 2
};

// Object: Enum Feature_SP.ESPPVPGame_NumericType
enum class ESPPVPGame_NumericType : uint8_t
{
	Absolute = 0,
	Percent = 1,
	ESPPVPGame_MAX = 2
};

// Object: Enum Feature_SP.HexagonCubicDirection
enum class EHexagonCubicDirection : uint8_t
{
	E_RIGHT_UP = 0,
	E_RIGHT = 1,
	E_RIGHT_DOWN = 2,
	E_LEFT_DOWN = 3,
	E_LEFT = 4,
	E_LEFT_UP = 5,
	E_MAX = 6
};

// Object: Enum Feature_SP.EPVPHextechOperationType
enum class EPVPHextechOperationType : uint8_t
{
	EHOT_Manual = 0,
	EHOT_OnlyAuto = 1,
	EHOT_MAX = 2
};

// Object: Enum Feature_SP.EPVPHextechTargetIndicatorType
enum class EPVPHextechTargetIndicatorType : uint8_t
{
	ETIT_EnemyLockedTarget = 0,
	ETIT_FriendLockedTarget = 1,
	ETIT_CircularRange = 2,
	ETIT_SquareRange = 3,
	ETIT_MAX = 4
};

// Object: Enum Feature_SP.EPVPHextechAOEType
enum class EPVPHextechAOEType : uint8_t
{
	AOET_CircularRange = 0,
	AOET_SquareRange = 1,
	AOET_MAX = 2
};

// Object: Enum Feature_SP.EPVPHextechCampType
enum class EPVPHextechCampType : uint8_t
{
	ECT_Enemy = 0,
	ECT_Friendly = 1,
	ECT_Any = 2,
	ECT_MAX = 3
};

// Object: Enum Feature_SP.EPVPHextechTargetType
enum class EPVPHextechTargetType : uint8_t
{
	ETT_NoTarget = 0,
	ETT_LockedTarget = 1,
	ETT_AreaOfEffect = 2,
	ETT_ExtendedTarget1 = 3,
	ETT_ExtendedTarget2 = 4,
	ETT_ExtendedTarget3 = 5,
	ETT_ExtendedTarget4 = 6,
	ETT_ExtendedTarget5 = 7,
	ETT_ExtendedTarget6 = 8,
	ETT_MAX = 9
};

// Object: Enum Feature_SP.EBaseLocationOfCapsule
enum class EBaseLocationOfCapsule : uint8_t
{
	Ignore = 0,
	Top = 1,
	Middle = 2,
	Bottom = 3,
	EBaseLocationOfCapsule_MAX = 4
};

// Object: Enum Feature_SP.EActorRepPriority
enum class EActorRepPriority : uint8_t
{
	AlwaysRepHigh = 1,
	OnlyRepToOwner = 2,
	EActorRepPriority_MAX = 3
};

// Object: Enum Feature_SP.ESPClassRepNodeMapping
enum class ESPClassRepNodeMapping : uint8_t
{
	NotRouted = 0,
	RelevantAllConnections = 1,
	RelevantTeamConnection = 2,
	RelevantOwnerConnection = 3,
	Dependent = 4,
	IsNetRelevantDependent = 5,
	AlwaysRelevantActorInfo = 6,
	Spatialize_Static = 7,
	Spatialize_Dynamic = 8,
	Spatialize_Dormancy = 9,
	ESPClassRepNodeMapping_MAX = 10
};

// Object: Enum Feature_SP.ESPResetInputTiming
enum class ESPResetInputTiming : uint8_t
{
	TaskStart = 0,
	TaskEnd = 1,
	NormalComplete = 2,
	Interrupted = 3,
	ESPResetInputTiming_MAX = 4
};

// Object: Enum Feature_SP.ESPRideRiddenMoveControlType
enum class ESPRideRiddenMoveControlType : uint8_t
{
	Default = 0,
	Aiming = 1,
	ESPRideRiddenMoveControlType_MAX = 2
};

// Object: Enum Feature_SP.ESPRideMoveZDirType
enum class ESPRideMoveZDirType : uint8_t
{
	Horizon = 0,
	Up = 1,
	Down = 2,
	ESPRideMoveZDirType_MAX = 3
};

// Object: Enum Feature_SP.ESPRideSysDismountCheckResult
enum class ESPRideSysDismountCheckResult : uint8_t
{
	Success = 0,
	WrongRiderPawnState = 1,
	WrongRiddenPawnState = 2,
	NotRiding = 3,
	InvalidRideSys = 4,
	UninitializedRideSys = 5,
	InvalidPawnsOrPlayerController = 6,
	Unknown = 7,
	ESPRideSysDismountCheckResult_MAX = 8
};

// Object: Enum Feature_SP.ESPRideSysMountCheckResult
enum class ESPRideSysMountCheckResult : uint8_t
{
	Success = 0,
	TooFar = 1,
	Blocked = 2,
	Riding = 3,
	WrongRiderPawnState = 4,
	WrongRiddenPawnState = 5,
	NotMountable = 6,
	InvalidRideSys = 7,
	UninitializedRideSys = 8,
	InvalidPawnsOrPlayerController = 9,
	InvalidRiddenSys = 10,
	NoSeat = 11,
	Unknown = 12,
	ESPRideSysMountCheckResult_MAX = 13
};

// Object: Enum Feature_SP.ESPPetMountPos
enum class ESPPetMountPos : uint8_t
{
	Undefined = 0,
	Back = 1,
	Head = 2,
	OneHandSuspension = 3,
	TwoHandSuspension = 4,
	ESPPetMountPos_MAX = 5
};

// Object: Enum Feature_SP.ESPRotationMode
enum class ESPRotationMode : uint8_t
{
	None = 0,
	RotationInPlaceWithAiming = 1,
	ESPRotationMode_MAX = 2
};

// Object: Enum Feature_SP.ESPRotationAnimationType
enum class ESPRotationAnimationType : uint8_t
{
	ExternalControl = 0,
	UseIdleAsTurnPose = 1,
	UseWalkPoseAsTurnPose = 2,
	UseTurnPose = 3,
	UseMontage = 4,
	UseAngleToDecide = 5,
	ESPRotationAnimationType_MAX = 6
};

// Object: Enum Feature_SP.ESPRotationSpeedType
enum class ESPRotationSpeedType : uint8_t
{
	Invalid = 0,
	NormalSpeed = 1,
	LowSpeed = 2,
	MediumSpeed = 3,
	HighSpeed = 4,
	ESPRotationSpeedType_MAX = 5
};

// Object: Enum Feature_SP.ESPRotationSpeedSetting
enum class ESPRotationSpeedSetting : uint8_t
{
	Invalid = 0,
	Customized = 1,
	Predefined = 2,
	ESPRotationSpeedSetting_MAX = 3
};

// Object: Enum Feature_SP.ESPRotationDriveMode
enum class ESPRotationDriveMode : uint8_t
{
	Invalid = 0,
	SpeedBased = 1,
	TimeBased = 2,
	ESPRotationDriveMode_MAX = 3
};

// Object: Enum Feature_SP.ESPRotationControlStopReason
enum class ESPRotationControlStopReason : uint8_t
{
	Finished = 0,
	UserRequested = 1,
	InterruptedByHigherPriority = 2,
	InterruptedBySameRotation = 3,
	ForceInterrupted = 4,
	ESPRotationControlStopReason_MAX = 5
};

// Object: Enum Feature_SP.ESP_TeleportDestType
enum class ESP_TeleportDestType : uint8_t
{
	OW = 0,
	Dungeon = 1,
	ESP_MAX = 2
};

// Object: Enum Feature_SP.ESignificanceDebugMode
enum class ESignificanceDebugMode : uint8_t
{
	None = 0,
	Client = 1,
	Server = 2,
	Standalone = 3,
	ESignificanceDebugMode_MAX = 4
};

// Object: Enum Feature_SP.ESPSignificancePostProcessComputeType
enum class ESPSignificancePostProcessComputeType : uint8_t
{
	ClientPerformanceStatPostProcess = 0,
	ClientDeviceLevelPostProcess = 1,
	ServerPerformanceStatPostProcess = 2,
	ESPSignificancePostProcessComputeType_MAX = 3
};

// Object: Enum Feature_SP.ESPSignificanceComputeType
enum class ESPSignificanceComputeType : uint8_t
{
	ClientFixedImportanceZero = 0,
	ClientFixedImportanceOne = 1,
	ClientLocalPlayerFixedImportanceOne = 2,
	ClientViewImportance = 3,
	ClientViewImportance_DistanceOnly = 4,
	ClientViewImportance_ScreenRatioOnly = 5,
	ServerFixedImportanceZero = 6,
	ServerFixedImportanceOne = 7,
	ServerCharacterFixedImportanceOne = 8,
	ServerDistanceRelevantImportance = 9,
	ServerTileRelevantImportance = 10,
	ESPSignificanceComputeType_MAX = 11
};

// Object: Enum Feature_SP.ESPRootOffsetConfigMode
enum class ESPRootOffsetConfigMode : uint8_t
{
	Auto = 0,
	Template = 1,
	Custom = 2,
	ESPRootOffsetConfigMode_MAX = 3
};

// Object: Enum Feature_SP.ESPAbilityActivateReason
enum class ESPAbilityActivateReason : uint8_t
{
	Default = 0,
	Reactivate = 1,
	ESPAbilityActivateReason_MAX = 2
};

// Object: Enum Feature_SP.ESPAbilityExclusiveSkillTypeServerEnum
enum class ESPAbilityExclusiveSkillTypeServerEnum : uint8_t
{
	SPExclusiveAbilityTag_None = 0,
	SPExclusiveAbilityTag_Damage = 1,
	SPExclusiveAbilityTag_Toughness = 2,
	SPExclusiveAbilityTag_Heal = 3,
	SPExclusiveAbilityTag_Purification = 4,
	SPExclusiveAbilityTag_Buff = 5,
	SPExclusiveAbilityTag_Debuff = 6,
	SPExclusiveAbilityTag_Shiled = 7,
	SPExclusiveAbilityTag_Charge = 8,
	SPExclusiveAbilityTag_Contrl = 9,
	SPExclusiveAbilityTag_Exploreing = 10,
	SPExclusiveAbilityTag_MAX = 11
};

// Object: Enum Feature_SP.SPAbilityScopeInvalidBehaviorServerEnum
enum class ESPAbilityScopeInvalidBehaviorServerEnum : uint8_t
{
	SPAbilityScopeInvalidBehavior_Default = 0,
	SPAbilityScopeInvalidBehavior_FindNewTarget = 1,
	SPAbilityScopeInvalidBehavior_InterruptAbility = 2,
	SPAbilityScopeInvalidBehavior_MAX = 3
};

// Object: Enum Feature_SP.SPAbilityScopePolicyServerEnum
enum class ESPAbilityScopePolicyServerEnum : uint8_t
{
	SPAbilityScopePolicy_AllAreInScope = 0,
	SPAbilityScopePolicy_AtLeastOneTargetInScope = 1,
	SPAbilityScopePolicy_MAX = 2
};

// Object: Enum Feature_SP.ESPAbilityAITagServerEnum
enum class ESPAbilityAITagServerEnum : uint8_t
{
	SPAbilityAITag_None = 0,
	SPAbilityAITag_Recovery = 1,
	SPAbilityAITag_Charge = 2,
	SPAbilityAITag_Shield = 3,
	SPAbilityAITag_Taunt = 4,
	SPAbilityAITag_Cleanup = 5,
	SPAbilityAITag_Fighting = 6,
	SPAbilityAITag_MAX = 7
};

// Object: Enum Feature_SP.ESPAbilityTypeServerEnum
enum class ESPAbilityTypeServerEnum : uint8_t
{
	SPAT_Ability_None = 0,
	SPAT_Ability_NormalAtk = 1,
	SPAT_Ability_Ultimate = 2,
	SPAT_Ability_Exclusive = 3,
	SPAT_Ability_Partner = 4,
	SPAT_Ability_Common = 5,
	SPAT_Ability_Counterattack = 6,
	SPAT_Ability_MAX = 7
};

// Object: Enum Feature_SP.ESPSkillTypeServerEnum
enum class ESPSkillTypeServerEnum : uint8_t
{
	SPSkillType_Undefined = 0,
	SPSkillType_Melee = 1,
	SPSkillType_Range = 2,
	SPSkillType_Hit = 3,
	SPSkillType_MAX = 4
};

// Object: Enum Feature_SP.ESPAbilityInheritReason
enum class ESPAbilityInheritReason : uint8_t
{
	InheritGroup = 0,
	EnterMorph = 1,
	ExitMorph = 2,
	InheritType = 3,
	ESPAbilityInheritReason_MAX = 4
};

// Object: Enum Feature_SP.ESPAbilityClearReason
enum class ESPAbilityClearReason : uint8_t
{
	Destroy = 0,
	Dead = 1,
	ESPAbilityClearReason_MAX = 2
};

// Object: Enum Feature_SP.ESPAbilityPolicyEnum
enum class ESPAbilityPolicyEnum : uint8_t
{
	None = 0,
	Prediction = 1,
	ESPAbilityPolicyEnum_MAX = 2
};

// Object: Enum Feature_SP.ESPAbilityTargetType
enum class ESPAbilityTargetType : uint8_t
{
	Self = 0,
	CalledMonster = 1,
	Master = 3,
	EnemyInRange = 4,
	Instigator = 5,
	EventTarget = 6,
	EventInstigator = 7,
	AllPetInTeam = 8,
	MaxAtkPetInRange = 10,
	SummonMaster = 11,
	MemberInRange = 12,
	BossInThreat = 13,
	MorphPlayer = 14,
	PlayerOrMorphPlayer = 15,
	CharmedPet = 16,
	All = 50,
	ESPAbilityTargetType_MAX = 51
};

// Object: Enum Feature_SP.ESPAbilityAreaTraceTargetType
enum class ESPAbilityAreaTraceTargetType : uint8_t
{
	None = 0,
	Ally = 1,
	Enemy = 2,
	Master = 3,
	ESPAbilityAreaTraceTargetType_MAX = 4
};

// Object: Enum Feature_SP.ESPActionReason
enum class ESPActionReason : uint8_t
{
	DefaultActivated = 0,
	LayerChanged = 1,
	LevelChanged = 2,
	DataReload = 3,
	Reactivated = 4,
	Modified = 5,
	ESPActionReason_MAX = 6
};

// Object: Enum Feature_SP.ESPBuffResult
enum class ESPBuffResult : uint8_t
{
	Passed = 0,
	Failed_Mutex = 1,
	Failed_MaxLayer = 2,
	Failed_ExternalReason = 3,
	Failed_CanNotFindInstance = 4,
	Failed_NonPositiveLayer = 5,
	ESPBuffResult_MAX = 6
};

// Object: Enum Feature_SP.ESPBuffStackRule
enum class ESPBuffStackRule : uint8_t
{
	OverrideStartTime = 0,
	RefreshStartTimeWhenReduceLayer = 1,
	ExtendDuration = 2,
	IndependentDuration = 3,
	MaintainDuration = 4,
	ESPBuffStackRule_MAX = 5
};

// Object: Enum Feature_SP.ESPScopeType
enum class ESPScopeType : uint8_t
{
	All = 0,
	InStage = 1,
	OutStage = 2,
	InBackPack = 3,
	InTerminal = 4,
	IsMorph = 5,
	ESPScopeType_MAX = 6
};

// Object: Enum Feature_SP.ESPAbilityExclusionRule
enum class ESPAbilityExclusionRule : uint8_t
{
	OnlyActive = 0,
	All = 1,
	ESPAbilityExclusionRule_MAX = 2
};

// Object: Enum Feature_SP.EShootSurfaceShape
enum class EShootSurfaceShape : uint8_t
{
	Rectangle = 0,
	Circle = 1,
	EShootSurfaceShape_MAX = 2
};

// Object: Enum Feature_SP.EAverageDirection
enum class EAverageDirection : uint8_t
{
	Horizon = 0,
	Vertical = 1,
	EAverageDirection_MAX = 2
};

// Object: Enum Feature_SP.ESPProjectileDistributionRule
enum class ESPProjectileDistributionRule : uint8_t
{
	Random = 0,
	Average = 1,
	Gaussian = 2,
	ESPProjectileDistributionRule_MAX = 3
};

// Object: Enum Feature_SP.ESPProjectileType
enum class ESPProjectileType : uint8_t
{
	Parabola = 0,
	TracingTarget = 1,
	Reversible = 2,
	ESPProjectileType_MAX = 3
};

// Object: Enum Feature_SP.ESPSkillAbilityStartResult
enum class ESPSkillAbilityStartResult : uint8_t
{
	InvalidTarget = 0,
	FailedCustomCheck = 1,
	CooldownNotExpired = 2,
	CannotInterruptCurrentAbility = 3,
	NotAllowedAsPassive = 4,
	PassiveMaxStacksReached = 5,
	InternalSystemsError = 6,
	AsyncProcessing = 7,
	ForwardedToServer = 8,
	InvalidParameter = 9,
	Success = 10,
	ESPSkillAbilityStartResult_MAX = 11
};

// Object: Enum Feature_SP.ESPSlotButtonType
enum class ESPSlotButtonType : uint8_t
{
	Invalid = 0,
	ClickTrigger = 1,
	PressingAndLeaveCancel = 2,
	PressingAndLeaveTrigger = 3,
	NoSlot = 4,
	ESPSlotButtonType_MAX = 5
};

// Object: Enum Feature_SP.ESPAbilityCooldownType
enum class ESPAbilityCooldownType : uint8_t
{
	None = 0,
	AllCooldown = 1,
	CurrentCooldown = 2,
	BaseCooldown = 3,
	ESPAbilityCooldownType_MAX = 4
};

// Object: Enum Feature_SP.ESPCooldownFactorType
enum class ESPCooldownFactorType : uint8_t
{
	None = 0,
	ApplyReduction = 1,
	Override = 2,
	ESPCooldownFactorType_MAX = 3
};

// Object: Enum Feature_SP.ESPSkillType
enum class ESPSkillType : uint8_t
{
	Normal = 0,
	Encounter = 1,
	FarSkill = 2,
	ESPSkillType_MAX = 3
};

// Object: Enum Feature_SP.EAbilityInstanceOperation
enum class EAbilityInstanceOperation : uint8_t
{
	Give = 0,
	Remove = 1,
	EAbilityInstanceOperation_MAX = 2
};

// Object: Enum Feature_SP.EAbilityActivateResult
enum class EAbilityActivateResult : uint8_t
{
	Passed = 0,
	Failed_Activating = 1,
	Failed_ConsumeNotEnough = 2,
	Failed_ConditionNotMeet = 3,
	Failed_CooldownNotEnough = 4,
	Failed_AbleNotFound = 5,
	Failed_AbleActivateFailed = 6,
	Failed_ServerInstanceMissing = 7,
	Failed_ProbCheckFail = 8,
	Failed_AuthorityCheckFail = 9,
	Failed_DontHaveAbility = 10,
	Failed_BlockedByHigherPriority = 11,
	Failed_Disable = 12,
	Failed_ScopeNotMeet = 13,
	Failed_CanNotAutoActivate = 14,
	Invalid = 15,
	EAbilityActivateResult_MAX = 16
};

// Object: Enum Feature_SP.ESPBuffTag
enum class ESPBuffTag : uint16_t
{
	None = 0,
	RemoveOnDungeonExit = 1,
	BuffTag = 2,
	BuffTag = 4,
	BuffTag = 8,
	BuffTag = 16,
	BuffTag = 32,
	BuffTag = 64,
	BuffTag = 128,
	BuffTag = 256,
	BuffTag = 512,
	BuffTag = 1024,
	ESPBuffTag_MAX = 1025
};

// Object: Enum Feature_SP.ESkinAssetType
enum class ESkinAssetType : uint8_t
{
	SkinPath = 0,
	AnimationPath = 1,
	EffectPath = 2,
	SpawnEffect = 3,
	ConstantlyEffect = 4,
	ABPPath = 5,
	PartialMeshPath = 6,
	ActorBPPath = 7,
	ESkinAssetType_MAX = 8
};

// Object: Enum Feature_SP.ESPSummonEndMode
enum class ESPSummonEndMode : uint8_t
{
	None = 0,
	Kill = 1,
	Destroy = 2,
	ESPSummonEndMode_MAX = 3
};

// Object: Enum Feature_SP.ESPStaminaSysConsumingQueryResult
enum class ESPStaminaSysConsumingQueryResult : uint8_t
{
	Success = 0,
	InvalidConfig = 1,
	InvalidSys = 2,
	InvalidCacheStates = 3,
	BelowMinDemand = 4,
	NotEnough = 5,
	StateMachineNotAllowCosuming = 6,
	Penalized = 7,
	ESPStaminaSysConsumingQueryResult_MAX = 8
};

// Object: Enum Feature_SP.ESPPredefinedStaminaConsumeEffect
enum class ESPPredefinedStaminaConsumeEffect : uint8_t
{
	None = 0,
	Climb = 1,
	Sprint = 2,
	Glide = 3,
	ClimbJump = 4,
	MeleeAttack = 5,
	SwimIdle = 6,
	SwimSlow = 7,
	SwimFast = 8,
	ESPPredefinedStaminaConsumeEffect_MAX = 9
};

// Object: Enum Feature_SP.ESPBurstDurationFactorType
enum class ESPBurstDurationFactorType : uint8_t
{
	None = 0,
	ApplyReduction = 1,
	Override = 2,
	ESPBurstDurationFactorType_MAX = 3
};

// Object: Enum Feature_SP.ESPStatusEffectState
enum class ESPStatusEffectState : uint8_t
{
	None = 0,
	Accumulated = 1,
	Bursting = 2,
	ESPStatusEffectState_MAX = 3
};

// Object: Enum Feature_SP.ESPSwimStateType
enum class ESPSwimStateType : uint8_t
{
	IdleSwim = 0,
	SlowSwim = 1,
	FastSwim = 2,
	ESPSwimStateType_MAX = 3
};

// Object: Enum Feature_SP.ESPTargetFilterSightMode
enum class ESPTargetFilterSightMode : uint8_t
{
	NoLineOfSight = 0,
	LineOfSight = 1,
	ActorLineOfSight = 2,
	ESPTargetFilterSightMode_MAX = 3
};

// Object: Enum Feature_SP.ESPFilterScreenValueType
enum class ESPFilterScreenValueType : uint8_t
{
	FixedValue = 0,
	Percentage = 1,
	ESPFilterScreenValueType_MAX = 2
};

// Object: Enum Feature_SP.ESPFilterScreenType
enum class ESPFilterScreenType : uint8_t
{
	Circle = 0,
	Rect = 1,
	ESPFilterScreenType_MAX = 2
};

// Object: Enum Feature_SP.ESPTargetAssistLogic
enum class ESPTargetAssistLogic : uint8_t
{
	InValid = 0,
	Weak = 1,
	Lock = 2,
	ESPTargetAssistLogic_MAX = 3
};

// Object: Enum Feature_SP.ESPNavAreaType
enum class ESPNavAreaType : uint8_t
{
	NullArea = 0,
	Land = 1,
	LightWater = 2,
	DepWater = 3,
	SOCBuilding = 4,
	POI = 5,
	PlayerBuilding = 6,
	Wall = 7,
	Tree = 8,
	Max = 9
};

// Object: Enum Feature_SP.ESPWeaponPoseType
enum class ESPWeaponPoseType : uint8_t
{
	LeftHand = 0,
	RightHand = 1,
	BothHand = 2,
	ESPWeaponPoseType_MAX = 3
};

// Object: Enum Feature_SP.ESPCanFireResult
enum class ESPCanFireResult : uint8_t
{
	Success = 0,
	WeaponOwnerNotValid = 1,
	AmmoNotEnough = 2,
	StateNotReady = 3,
	Hidden = 4,
	CharacterDying = 5,
	PullingBolt = 6,
	CharRespBtnUnClickable = 7,
	InventoryComponentNotValid = 8,
	ShouldNotFire = 9,
	Other = 10,
	ESPCanFireResult_MAX = 11
};

// Object: Enum Feature_SP.ESPComboInputMode
enum class ESPComboInputMode : uint8_t
{
	Pressed = 0,
	Hold = 1,
	Auto = 2,
	ESPComboInputMode_MAX = 3
};

// Object: Enum Feature_SP.ESPComboIndexMode
enum class ESPComboIndexMode : uint8_t
{
	Next = 0,
	Prev = 1,
	Index = 2,
	First = 3,
	Last = 4,
	ESPComboIndexMode_MAX = 5
};

// Object: Enum Feature_SP.ESPComboActiveMode
enum class ESPComboActiveMode : uint8_t
{
	Immediately = 0,
	AtEnd = 1,
	ESPComboActiveMode_MAX = 2
};

// Object: Enum Feature_SP.ESPWeaponProjectileDamageType
enum class ESPWeaponProjectileDamageType : uint8_t
{
	Single = 0,
	AOE = 1,
	NoDamage = 2,
	ESPWeaponProjectileDamageType_MAX = 3
};

// Object: Enum Feature_SP.ESPWeaponProjectileCollisionType
enum class ESPWeaponProjectileCollisionType : uint8_t
{
	Line = 0,
	Sphere = 1,
	ESPWeaponProjectileCollisionType_MAX = 2
};

// Object: Enum Feature_SP.ESPWeaponSkinAssetType
enum class ESPWeaponSkinAssetType : uint8_t
{
	Mesh = 0,
	Anim = 1,
	Projectile = 2,
	OverloadMuzzleFX_1P = 3,
	OverloadMuzzleFX_3P = 4,
	ESPWeaponSkinAssetType_MAX = 5
};

// Object: Enum Feature_SP.EWeaponVisibilityReason
enum class EWeaponVisibilityReason : uint8_t
{
	None = 0,
	Ability = 1,
	Animation = 2,
	Cutscene = 3,
	UI = 4,
	Gameplay = 5,
	GM = 6,
	Custom = 7,
	DungeonUI = 8,
	EWeaponVisibilityReason_MAX = 9
};

// Object: Enum Feature_SP.EWeaponCombatState
enum class EWeaponCombatState : uint8_t
{
	ExitCombat = 0,
	EnterCombat = 1,
	KeepState = 2,
	EWeaponCombatState_MAX = 3
};

// Object: Enum Feature_SP.EWeaponItemHiddenFlags
enum class EWeaponItemHiddenFlags : uint8_t
{
	None = 0,
	UICrossHair = 1,
	UISwitching = 2,
	UIQuickSwitch = 4,
	EWeaponItemHiddenFlags_MAX = 5
};

// Package: Feature_FarmBase
// Enums: 1

// Object: Enum Feature_FarmBase.EFarmDialogType
enum class EFarmDialogType : uint8_t
{
	EDT_Dialog = 0,
	EDT_Hint = 1,
	EDT_MAX = 2
};

// Package: Feature_FarmCrazy
// Enums: 25

// Object: Enum Feature_FarmCrazy.EFarmCrazyBrickContactFace
enum class EFarmCrazyBrickContactFace : uint8_t
{
	Bottom = 0,
	Top = 1,
	Left = 2,
	Right = 3,
	Front = 4,
	Back = 5,
	All = 6,
	None = 7,
	EFarmCrazyBrickContactFace_MAX = 8
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyIaResCode
enum class EFarmCrazyIaResCode : uint8_t
{
	Success = 0,
	IsBusy = 1,
	NoActor = 2,
	Other = 3,
	EFarmCrazyIaResCode_MAX = 4
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyIaOperation
enum class EFarmCrazyIaOperation : uint8_t
{
	Enter = 0,
	Exit = 1,
	Other = 2,
	EFarmCrazyIaOperation_MAX = 3
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyBpFlag
enum class EFarmCrazyBpFlag : uint8_t
{
	OnFarmTriggerBeginOverlap = 0,
	OnFarmTriggerEndOverlap = 1,
	OnReqOperate_Server = 2,
	OnOperateRes_Client = 3,
	Server_OnReqOperate = 4,
	OnMulticastAction = 5,
	EFarmCrazyBpFlag_MAX = 6
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyBrickPlaceResult
enum class EFarmCrazyBrickPlaceResult : uint8_t
{
	OK = 0,
	OutOfBounds = 1,
	Overlap = 2,
	Floating = 3,
	InvalidInput = 4,
	InvalidRotation = 5,
	NotInitialized = 6,
	NoValidAttach = 7,
	FrontLocked = 8,
	UpLocked = 9,
	DuplicateUID = 10,
	BrickNotFound = 11,
	InvalidBrickType = 12,
	SpawnFailed = 13,
	InvalidBrickSize = 14,
	EFarmCrazyBrickPlaceResult_MAX = 15
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyCustomMovementMode
enum class EFarmCrazyCustomMovementMode : uint8_t
{
	CMOVE_FarmCrazy_None = 0,
	CMOVE_FarmCrazy_RodSurfing = 10,
	CMOVE_FarmCrazy_MAX = 11
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyCharBpFlag
enum class EFarmCrazyCharBpFlag : uint8_t
{
	SkipInteractiveComp = 0,
	GetInteractiveComponentFromUid = 1,
	Update = 2,
	Log = 3,
	EFarmCrazyCharBpFlag_MAX = 4
};

// Object: Enum Feature_FarmCrazy.EEntityUidType
enum class EEntityUidType : uint8_t
{
	Default = 0,
	Crop = 1,
	EEntityUidType_MAX = 2
};

// Object: Enum Feature_FarmCrazy.ECropNodeType
enum class ECropNodeType : uint8_t
{
	Ground = 0,
	GroundDot = 1,
	Trunk = 2,
	TrunkDot = 3,
	Branch = 4,
	BranchDot = 5,
	Leaf = 6,
	LeafDot = 7,
	Fruit = 8,
	ECropNodeType_MAX = 9
};

// Object: Enum Feature_FarmCrazy.EAttachScaleType
enum class EAttachScaleType : uint8_t
{
	FollowParent = 0,
	StartAsTargetReachHeight = 1,
	EAttachScaleType_MAX = 2
};

// Object: Enum Feature_FarmCrazy.ECropGrowType
enum class ECropGrowType : uint8_t
{
	TopGrow = 0,
	WholeGrow = 1,
	StageGrow = 2,
	ECropGrowType_MAX = 3
};

// Object: Enum Feature_FarmCrazy.EAttachDistributionType
enum class EAttachDistributionType : uint8_t
{
	CenterAvg = 0,
	Mirror = 1,
	LocOffset = 2,
	LocOffsetWithNormal = 3,
	AbsoluteLocOffset = 4,
	EAttachDistributionType_MAX = 5
};

// Object: Enum Feature_FarmCrazy.EAttachType
enum class EAttachType : uint8_t
{
	DotList = 0,
	HeightRangeRandom = 1,
	EAttachType_MAX = 2
};

// Object: Enum Feature_FarmCrazy.EAttachTargetType
enum class EAttachTargetType : uint8_t
{
	Ground = 0,
	Trunk = 1,
	Branch = 2,
	BranchRoot = 3,
	Leaf = 4,
	LeafRoot = 5,
	EAttachTargetType_MAX = 6
};

// Object: Enum Feature_FarmCrazy.EFishObstacleType
enum class EFishObstacleType : uint8_t
{
	Static = 0,
	Dynamic = 1,
	EFishObstacleType_MAX = 2
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyScaleFinishAction
enum class EFarmCrazyScaleFinishAction : uint8_t
{
	None = 0,
	Hidden = 1,
	Destroy = 2,
	EFarmCrazyScaleFinishAction_MAX = 3
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyCalcMethod
enum class EFarmCrazyCalcMethod : uint8_t
{
	Dist3D = 0,
	Dist2D = 1,
	EFarmCrazyCalcMethod_MAX = 2
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyScreenFrustumMethod
enum class EFarmCrazyScreenFrustumMethod : uint8_t
{
	AABB = 0,
	OBB = 1,
	Sphere = 2,
	Capsule = 3,
	EFarmCrazyScreenFrustumMethod_MAX = 4
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyIsmColBpFlag
enum class EFarmCrazyIsmColBpFlag : uint8_t
{
	RecycleComp = 0,
	Log = 1,
	LogWarning = 2,
	LogUpdate = 3,
	EFarmCrazyIsmColBpFlag_MAX = 4
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyIsmVisualBpFlag
enum class EFarmCrazyIsmVisualBpFlag : uint8_t
{
	RecycleComp = 0,
	Log = 1,
	LogWarning = 2,
	LogUpdate = 3,
	EFarmCrazyIsmVisualBpFlag_MAX = 4
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyComponentType
enum class EFarmCrazyComponentType : uint8_t
{
	Default = 1,
	Fruit = 2,
	Plant = 3,
	Effect = 4,
	EFarmCrazyComponentType_MAX = 5
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyPropSlot
enum class EFarmCrazyPropSlot : uint8_t
{
	RightHand = 0,
	LeftHand = 1,
	Invalid = 99,
	EFarmCrazyPropSlot_MAX = 100
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyPropType
enum class EFarmCrazyPropType : uint8_t
{
	Identity = 0,
	OnHand = 1,
	Invalid = 99,
	EFarmCrazyPropType_MAX = 100
};

// Object: Enum Feature_FarmCrazy.EFarmCrazyPropState
enum class EFarmCrazyPropState : uint8_t
{
	Equip = 0,
	PrepareUse = 1,
	AfterUse = 2,
	InOperation = 3,
	InValid = 99,
	EFarmCrazyPropState_MAX = 100
};

// Object: Enum Feature_FarmCrazy.ERodSurfingJumpPhase
enum class ERodSurfingJumpPhase : uint8_t
{
	None = 0,
	Ascending = 1,
	Descending = 2,
	Bouncing = 3,
	ERodSurfingJumpPhase_MAX = 4
};

// Package: Feature_Farm
// Enums: 15

// Object: Enum Feature_Farm.EFarmMoveToResult
enum class EFarmMoveToResult : uint8_t
{
	Success = 0,
	Abort = 1,
	Failure = 2,
	Blocked = 3,
	EFarmMoveToResult_MAX = 4
};

// Object: Enum Feature_Farm.EFarmIaResCode
enum class EFarmIaResCode : uint8_t
{
	Success = 0,
	IsBusy = 1,
	NoActor = 2,
	Other = 3,
	EFarmIaResCode_MAX = 4
};

// Object: Enum Feature_Farm.EFarmIaState
enum class EFarmIaState : uint8_t
{
	None = 0,
	Entering = 1,
	Gaming = 2,
	Exiting = 3,
	Creating = 4,
	Destroy = 5,
	EFarmIaState_MAX = 6
};

// Object: Enum Feature_Farm.EFarmIaAniTargetType
enum class EFarmIaAniTargetType : uint8_t
{
	NoTarget = 0,
	Point = 1,
	RelativeTrans = 2,
	EFarmIaAniTargetType_MAX = 3
};

// Object: Enum Feature_Farm.EFarmIaOperation
enum class EFarmIaOperation : uint8_t
{
	Enter = 0,
	Exit = 1,
	Other = 2,
	EFarmIaOperation_MAX = 3
};

// Object: Enum Feature_Farm.EFarmBpFlag
enum class EFarmBpFlag : uint8_t
{
	OnFarmActorBeginOverlap = 0,
	OnFarmActorEndOverlap = 1,
	OnFirstCharEnterActor = 2,
	OnFirstCharLeaveActor = 3,
	OnFarmTriggerBeginOverlap = 4,
	OnFarmTriggerEndOverlap = 5,
	GetInteractiveIconInfo = 6,
	OnGridFocused = 7,
	CanOperate_Client = 8,
	OnClickInteractButton = 9,
	OnReqEnter_Server = 10,
	OnReqExit_Server = 11,
	OnReqOperate_Server = 12,
	CanEnterSeat_Server = 13,
	CanExitSeat_Server = 14,
	CharDoEnter_Server = 15,
	CharDoExit_Server = 16,
	OnCharStateChange_Server = 17,
	OnCharStateChange_Client = 18,
	OnOperateRes_Client = 19,
	OnCharacterReset = 20,
	OnInteractBindEnterEnd = 21,
	OnInteractBindExitEnd = 22,
	Server_OnReqOperate = 23,
	OnMulticastAction = 24,
	OnAdaptionComplete = 25,
	SetInteractiveActor = 26,
	LogProxyAnim = 27,
	GetNextAnimId = 28,
	PrePickUp = 29,
	PostPickUp = 30,
	PrePutDown = 31,
	PostPutDown = 32,
	EFarmBpFlag_MAX = 33
};

// Object: Enum Feature_Farm.EFarmAnimalBpFlag
enum class EFarmAnimalBpFlag : uint8_t
{
	Tick = 0,
	FixMeshYaw = 1,
	LogProxyAnim = 2,
	GetNextAnimId = 3,
	EFarmAnimalBpFlag_MAX = 4
};

// Object: Enum Feature_Farm.EFarmAnimalAnimId
enum class EFarmAnimalAnimId : uint8_t
{
	None = 0,
	Stand = 1,
	Walk = 2,
	Eat = 3,
	Sit = 4,
	SitDown = 5,
	StandUp = 6,
	Produce = 7,
	StandIdle = 8,
	StandIdle2 = 9,
	Fly = 10,
	FlyStart = 11,
	FlyDown = 12,
	EFarmAnimalAnimId_MAX = 13
};

// Object: Enum Feature_Farm.EFarmAnimalAction
enum class EFarmAnimalAction : uint8_t
{
	Rest = 0,
	Walk = 1,
	Eat = 2,
	Produce = 3,
	Idle1 = 4,
	Idle2 = 5,
	Idle3 = 6,
	Idle4 = 7,
	Idle5 = 8,
	EFarmAnimalAction_MAX = 9
};

// Object: Enum Feature_Farm.EFarmIaReactionProcess
enum class EFarmIaReactionProcess : uint8_t
{
	None = 0,
	Enter = 1,
	Standby = 2,
	Exit = 4,
	EFarmIaReactionProcess_MAX = 5
};

// Object: Enum Feature_Farm.EFarmCharBpFlag
enum class EFarmCharBpFlag : uint8_t
{
	SkipInteractiveComp = 0,
	GetInteractiveComponentFromUid = 1,
	IsPlayAnim = 2,
	CanEnterSeat = 3,
	CanOperate = 4,
	Log = 5,
	EFarmCharBpFlag_MAX = 6
};

// Object: Enum Feature_Farm.EFarmScaleFinishAction
enum class EFarmScaleFinishAction : uint8_t
{
	None = 0,
	Hidden = 1,
	Destory = 2,
	EFarmScaleFinishAction_MAX = 3
};

// Object: Enum Feature_Farm.EFarmIndicatorBpFlag
enum class EFarmIndicatorBpFlag : uint8_t
{
	UpdateIndicator = 0,
	ShowRootWidget = 1,
	UpdateWidget = 2,
	EFarmIndicatorBpFlag_MAX = 3
};

// Object: Enum Feature_Farm.EFarmMoveToStatus
enum class EFarmMoveToStatus : uint8_t
{
	None = 0,
	Paused = 1,
	Moving = 2,
	EFarmMoveToStatus_MAX = 3
};

// Object: Enum Feature_Farm.EFarmVillagerBpFlag
enum class EFarmVillagerBpFlag : uint8_t
{
	Tick = 0,
	EFarmVillagerBpFlag_MAX = 1
};

// Package: Feature_Home
// Enums: 1

// Object: Enum Feature_Home.EHomeHandHeldBpFlag
enum class EHomeHandHeldBpFlag : uint8_t
{
	None = 0,
	GetIsForbidReset = 1,
	PlayAPoseMontage = 2,
	PlayOperateMontage = 3,
	StartOperateAction = 4,
	StopOperateAction = 5,
	HideHandItem = 6,
	OnCharStateChange = 7,
	RecoverMontage = 8,
	EHomeHandHeldBpFlag_MAX = 9
};

// Package: Feature_JS
// Enums: 1

// Object: Enum Feature_JS.EJSRaceType
enum class EJSRaceType : uint8_t
{
	RaceGame = 0,
	PropGame = 1,
	EJSRaceType_MAX = 2
};

// Package: Feature_MCG
// Enums: 12

// Object: Enum Feature_MCG.EMCGCharActionState
enum class EMCGCharActionState : uint8_t
{
	None = 0,
	Max = 1
};

// Object: Enum Feature_MCG.EMoeAnimModule_MCGBossActionState
enum class EMoeAnimModule_MCGBossActionState : uint8_t
{
	Moe_Default = 0,
	Moe_BossCatch = 1,
	Moe_BossRelease = 2,
	Moe_BossExile = 3,
	Moe_BossStub = 4,
	Moe_BossClimb = 5,
	Moe_BossBreakBoard = 6,
	Moe_BossCommonAction = 7,
	Moe_BossSearchHidingOut = 8,
	Moe_MAX = 9
};

// Object: Enum Feature_MCG.EMoeAnimModule_MCGBossUpperBodyActionState
enum class EMoeAnimModule_MCGBossUpperBodyActionState : uint8_t
{
	Moe_Default = 0,
	Moe_BossAttack1Before = 1,
	Moe_BossCarrying = 2,
	Moe_CommonUpper = 3,
	Moe_MAX = 4
};

// Object: Enum Feature_MCG.EMoeAnimModule_MCGBaseLocomotionState
enum class EMoeAnimModule_MCGBaseLocomotionState : uint8_t
{
	MCG_Default = 0,
	MCG_Injuried = 1,
	MCG_Dying = 2,
	MCG_CaughtInBoss = 3,
	MCG_CaughtInChair = 4,
	MCG_CaughtInGourd = 5,
	MCG_Walking = 6,
	MCG_MAX = 7
};

// Object: Enum Feature_MCG.EMoeAnimModule_MCGUpperBodyActionState
enum class EMoeAnimModule_MCGUpperBodyActionState : uint8_t
{
	Moe_Default = 0,
	Moe_BossAttack1Before = 1,
	Moe_CommonSkillLoop = 2,
	Moe_RescuedSuccess = 3,
	Moe_RescueTeammates = 4,
	Moe_SlowClimbWindow = 5,
	Moe_FastClimbWindow = 6,
	Moe_SlowClimbBlock = 7,
	Moe_FastClimbBlock = 8,
	Moe_CaughtStruggle = 9,
	Moe_BeHit = 10,
	Moe_Idle2LastLand = 11,
	Moe_LastLand2Idle = 12,
	Moe_ReadingStarChart = 13,
	Moe_RescueFromAltar = 14,
	Moe_DecipherGate = 15,
	Moe_PlayerDeadForDying = 16,
	Moe_PlayerPutBoard = 17,
	Moe_SomersaultMove = 18,
	Moe_GrabHit = 19,
	Moe_BeGrabHit = 20,
	MCG_HealTeammates = 21,
	MCG_CommonInteract = 22,
	EMoeAnimModule_MAX = 23
};

// Object: Enum Feature_MCG.EMCGCustomMovementMode
enum class EMCGCustomMovementMode : uint8_t
{
	None = 0,
	DraculaBatFly = 1,
	EMCGCustomMovementMode_MAX = 2
};

// Object: Enum Feature_MCG.EMCGSlideAlongSurfaceType
enum class EMCGSlideAlongSurfaceType : uint8_t
{
	Default = 0,
	ReduceForward = 1,
	Redirect = 2,
	EMCGSlideAlongSurfaceType_MAX = 3
};

// Object: Enum Feature_MCG.EMCGPreparePhase
enum class EMCGPreparePhase : uint8_t
{
	GPP_WaitForGenerateProp = 0,
	GPP_GenerateProp = 1,
	GPP_PropFly = 2,
	GPP_PropStopFly = 3,
	GPP_PropGenerated = 4,
	GPP_MAX = 5
};

// Object: Enum Feature_MCG.EMCGGamePhase
enum class EMCGGamePhase : uint8_t
{
	GP_Idle = 0,
	GP_Prepare = 1,
	GP_Playing = 2,
	GP_Finish = 3,
	GP_End = 4,
	GP_MAX = 5
};

// Object: Enum Feature_MCG.EMCGInteractiveActorType
enum class EMCGInteractiveActorType : uint8_t
{
	None = 0,
	StarChart = 1,
	Altar = 2,
	Password = 3,
	Board = 4,
	Window = 5,
	TrumpAltar = 6,
	Gachapon = 7,
	Fountain = 8,
	Vehicle = 9,
	StarTeleport = 10,
	CubeTrapRescue = 11,
	CubeTrapDestroy = 12,
	BloodTrapDestroy = 13,
	Cellar = 14,
	Chest = 15,
	Hideout = 16,
	ChestDoor = 17,
	Reserved1 = 18,
	Reserved2 = 19,
	Reserved3 = 20,
	Reserved4 = 21,
	Reserved5 = 22,
	EMCGInteractiveActorType_MAX = 23
};

// Object: Enum Feature_MCG.EMCGCharMotionState
enum class EMCGCharMotionState : uint8_t
{
	None = 0,
	Max = 1
};

// Object: Enum Feature_MCG.EMCGSubstituteState
enum class EMCGSubstituteState : uint8_t
{
	None = 0,
	Flying = 1,
	Transfer = 2,
	End = 3,
	EMCGSubstituteState_MAX = 4
};

// Package: Feature_NR3E8
// Enums: 6

// Object: Enum Feature_NR3E8.ENR3E8BuildingType
enum class ENR3E8BuildingType : uint8_t
{
	None = 0,
	Player = 1,
	Consume = 2,
	Buff = 3,
	Tax = 4,
	Interaction = 5,
	NewsEvent = 6,
	Dice = 7,
	Money = 8,
	Battle = 9,
	Visit = 10,
	Max = 11
};

// Object: Enum Feature_NR3E8.ENR3E8CameraMode
enum class ENR3E8CameraMode : uint8_t
{
	Follow = 0,
	Free = 1,
	ENR3E8CameraMode_MAX = 2
};

// Object: Enum Feature_NR3E8.ENR3E8AbilityVariantType
enum class ENR3E8AbilityVariantType : uint8_t
{
	NONE = 0,
	T_FLOAT = 1,
	T_BOOL = 2,
	T_INT64 = 3,
	ENR3E8AbilityVariantType_MAX = 4
};

// Object: Enum Feature_NR3E8.EUMGSplineCoordinateSpace
enum class EUMGSplineCoordinateSpace : uint8_t
{
	Local = 0,
	Viewport = 1,
	Screen = 2,
	EUMGSplineCoordinateSpace_MAX = 3
};

// Object: Enum Feature_NR3E8.ENR3E8SquareEdgeType
enum class ENR3E8SquareEdgeType : uint8_t
{
	None = 0,
	LeftCorner = 1,
	RightCorner = 2,
	UpperCorner = 3,
	LowerCorner = 4,
	Regular = 5,
	Max = 6
};

// Object: Enum Feature_NR3E8.EUMGSplineType
enum class EUMGSplineType : uint8_t
{
	Linear = 0,
	Curve = 1,
	EUMGSplineType_MAX = 2
};

// Package: Feature_NR3E
// Enums: 47

// Object: Enum Feature_NR3E.EUIExtensionAction
enum class EUIExtensionAction : uint8_t
{
	Added = 0,
	Removed = 1,
	EUIExtensionAction_MAX = 2
};

// Object: Enum Feature_NR3E.ENR3E2TrapStatus
enum class ENR3E2TrapStatus : uint8_t
{
	StepTrapOnClient = 0,
	CreateTrap = 1,
	StopCreateTrap = 2,
	InterruptProgress = 3,
	Rescue = 4,
	RescueByOther = 5,
	RescueSuccess = 6,
	UpdateShowRescueBtn = 7,
	StepTrapNotify = 8,
	ReportdBeRescueCharacter = 9,
	ClearTrapEffect = 10,
	ENR3E2TrapStatus_MAX = 11
};

// Object: Enum Feature_NR3E.ENR3E3TaskCompleteResult
enum class ENR3E3TaskCompleteResult : uint8_t
{
	Fail_DistanceTooFar = 0,
	Fail_OtherException = 1,
	SuccessHalf = 2,
	Success = 3,
	Reserved1 = 4,
	Reserved2 = 5,
	ENR3E3TaskCompleteResult_MAX = 6
};

// Object: Enum Feature_NR3E.EUIExtensionPointMatch
enum class EUIExtensionPointMatch : uint8_t
{
	ExactMatch = 0,
	PartialMatch = 1,
	EUIExtensionPointMatch_MAX = 2
};

// Object: Enum Feature_NR3E.ENR3E1GameStage
enum class ENR3E1GameStage : uint8_t
{
	None = 0,
	HideStage = 1,
	CatchStage = 2,
	ENR3E1GameStage_MAX = 3
};

// Object: Enum Feature_NR3E.ENR3E1SpaceDoorType
enum class ENR3E1SpaceDoorType : uint8_t
{
	Opening = 0,
	Closing = 1,
	ENR3E1SpaceDoorType_MAX = 2
};

// Object: Enum Feature_NR3E.ENR3E2RoleType
enum class ENR3E2RoleType : uint8_t
{
	Killer = 0,
	Police = 1,
	Civilian = 2,
	Default = 3,
	ENR3E2RoleType_MAX = 4
};

// Object: Enum Feature_NR3E.ENR3E2AttackType
enum class ENR3E2AttackType : uint8_t
{
	BeginThrow = 0,
	Throw = 1,
	BeginFire = 2,
	Fire = 3,
	Stick = 4,
	Trap = 5,
	ENR3E2AttackType_MAX = 6
};

// Object: Enum Feature_NR3E.ENR3E2PlaceableActorType
enum class ENR3E2PlaceableActorType : uint8_t
{
	Default = 0,
	BigStar = 1,
	SmallStar = 2,
	Guy = 3,
	ENR3E2PlaceableActorType_MAX = 4
};

// Object: Enum Feature_NR3E.ENR3E2CharacterTrapStatus
enum class ENR3E2CharacterTrapStatus : uint8_t
{
	Default = 0,
	CreateTrapIng = 1,
	StepTrapIng = 2,
	RescueIng = 3,
	RescueSuccess = 4,
	ENR3E2CharacterTrapStatus_MAX = 5
};

// Object: Enum Feature_NR3E.EMoeNR3EAIPreFrameStateFlag
enum class EMoeNR3EAIPreFrameStateFlag : uint8_t
{
	None = 0,
	bAlive = 1,
	EMoeNR3EAIPreFrameStateFlag_MAX = 2
};

// Object: Enum Feature_NR3E.ENR3E3CharacterSide
enum class ENR3E3CharacterSide : uint8_t
{
	ENR3E3CS_NONE = 0,
	ENR3E3CS_A = 1,
	ENR3E3CS_B = 2,
	ENR3E3CS_C = 3,
	ENR3E3CS_D = 4,
	ENR3E3CS_E = 5,
	ENR3E3CS_F = 6,
	ENR3E3CS_G = 7,
	ENR3E3CS_H = 8,
	ENR3E3CS_I = 9,
	ENR3E3CS_MAX = 10
};

// Object: Enum Feature_NR3E.ENR3E3CommercialCameraType
enum class ENR3E3CommercialCameraType : uint8_t
{
	NONE = 0,
	SceneCamera = 1,
	RenderTargetCamera = 2,
	ENR3E3CommercialCameraType_MAX = 3
};

// Object: Enum Feature_NR3E.ENR3E3WinPriority
enum class ENR3E3WinPriority : uint8_t
{
	UnWin = 0,
	TimeRunOut = 1,
	GamblerWin = 2,
	DeadCountPriority = 3,
	SkunkWin = 4,
	BountyHunterWin = 5,
	IronManWin = 6,
	JokerWin = 7,
	UrgentTaskFailed = 8,
	TaskComplete = 9,
	PuppetMasterWin = 10,
	PorterWin = 11,
	TrampWin = 12,
	NianNianWin = 13,
	ENR3E3WinPriority_MAX = 14
};

// Object: Enum Feature_NR3E.ENR3EGameWinType
enum class ENR3EGameWinType : uint8_t
{
	UnWin = 0,
	AllBadGuyDead = 1,
	TaskFinish = 2,
	TreasureGoalAchieved = 3,
	CrisisNightTimeOut = 4,
	GoodGuyWinSpace2 = 5,
	GoodGuyWinSpace3 = 6,
	GoodGuyWinSpace4 = 7,
	GoodGuyWinSpace5 = 8,
	AllGoodGuyDead = 51,
	TimeRunOut = 52,
	IncompleteEmergency = 53,
	CrisisNightAllGoodGuysDead = 54,
	DoubleSecretBlueWin = 55,
	DoubleSecretRedWin = 56,
	DoubleSecretBlueWinMoreSurvivor = 57,
	DoubleSecretRedWinMoreSurvivor = 58,
	DoubleSecretBlueWinMoreSurviveTime = 59,
	DoubleSecretRedWinMoreSurviveTime = 60,
	DoubleSecretTimeOut = 61,
	BountyHunterKillWin = 101,
	GamblerWin = 102,
	JokerWin = 103,
	SkunkWin = 104,
	PuppetMaster = 105,
	PorterWin = 106,
	IronManWin = 107,
	TrampWin = 108,
	PhotographerWin = 109,
	DreamcatcherWin = 110,
	DancerWin = 111,
	DeliveryWin = 112,
	SecretDetectiveWin = 113,
	NaoNaoWin = 114,
	NianNianWin = 115,
	ConspiratorsWin = 116,
	StingWin = 117,
	AgentWin = 118,
	VampireWin = 119,
	NeutralWinSpace3 = 120,
	NeutralWinSpace4 = 121,
	NeutralWinSpace5 = 122,
	Max = 255
};

// Object: Enum Feature_NR3E.EMOENR3EFSMStateChageType
enum class EMOENR3EFSMStateChageType : uint8_t
{
	Index = 0,
	ConfigName = 1,
	ClassName = 2,
	EMOENR3EFSMStateChageType_MAX = 3
};

// Object: Enum Feature_NR3E.ENR3E3MeetingOverlay
enum class ENR3E3MeetingOverlay : uint8_t
{
	None = 0,
	DayHunterShoot = 1,
	LastWordsN2D = 2,
	LastWordsD2N = 3,
	OpenForNight = 4,
	ENR3E3MeetingOverlay_MAX = 5
};

// Object: Enum Feature_NR3E.ENR3E3NightSubPhase
enum class ENR3E3NightSubPhase : uint8_t
{
	None = 0,
	NightStart = 1,
	RoleActing = 2,
	NightSettle = 3,
	HunterShoot = 4,
	NightEnd = 5,
	ENR3E3NightSubPhase_MAX = 6
};

// Object: Enum Feature_NR3E.ENR3E3SpaceDoorType
enum class ENR3E3SpaceDoorType : uint8_t
{
	Opening = 0,
	Closing = 1,
	ENR3E3SpaceDoorType_MAX = 2
};

// Object: Enum Feature_NR3E.ENR3E3StreamingLevelStatus
enum class ENR3E3StreamingLevelStatus : uint8_t
{
	NoLevel = 0,
	UnloadingLevel = 1,
	LoadingLevel = 2,
	LevelLoaded = 3,
	LevelShowed = 4,
	LevelHided = 5,
	ENR3E3StreamingLevelStatus_MAX = 6
};

// Object: Enum Feature_NR3E.ENR3E3TaskCompletionMethod
enum class ENR3E3TaskCompletionMethod : uint8_t
{
	Default = 0,
	GM = 1,
	Reserved1 = 2,
	Reserved2 = 3,
	ENR3E3TaskCompletionMethod_MAX = 4
};

// Object: Enum Feature_NR3E.ENR3E3UrgentTaskType
enum class ENR3E3UrgentTaskType : uint8_t
{
	Default = 0,
	ExtinguishFire = 1,
	FogClearance = 2,
	DoorClosure = 3,
	MealTime = 4,
	IllnessResponse = 5,
	Reserved1 = 6,
	Reserved2 = 7,
	Reserved3 = 8,
	ENR3E3UrgentTaskType_MAX = 9
};

// Object: Enum Feature_NR3E.ENR3E3TaskCompleteReason
enum class ENR3E3TaskCompleteReason : uint8_t
{
	Default = 0,
	FireMen_Complete = 1,
	AutoFinish = 2,
	Reserved1 = 3,
	Reserved2 = 4,
	ENR3E3TaskCompleteReason_MAX = 5
};

// Object: Enum Feature_NR3E.ERefreshUrgentTaskReason
enum class ERefreshUrgentTaskReason : uint8_t
{
	DefaultRefresh = 0,
	InitRefresh = 1,
	TaskTriggered = 2,
	TaskFinished = 3,
	CooldownReady = 4,
	CooldownChanged = 5,
	MeetingJoined = 6,
	MeetingLeft = 7,
	ForceRefresh = 8,
	ERefreshUrgentTaskReason_MAX = 9
};

// Object: Enum Feature_NR3E.ENR3E3TaskFinishStatus
enum class ENR3E3TaskFinishStatus : uint8_t
{
	Default = 0,
	IncompleteEmergency = 1,
	CompleteNormal = 2,
	Reserved1 = 3,
	Reserved2 = 4,
	ENR3E3TaskFinishStatus_MAX = 5
};

// Object: Enum Feature_NR3E.ENR3E3ButtonType
enum class ENR3E3ButtonType : uint8_t
{
	NONE = 0,
	Available = 1,
	Unavailable = 2,
	Cooling = 3,
	ENR3E3ButtonType_MAX = 4
};

// Object: Enum Feature_NR3E.ENR3EAvatarLight
enum class ENR3EAvatarLight : uint8_t
{
	Game = 0,
	GameFinish = 1,
	Meeting = 2,
	Report = 3,
	BeKilled = 4,
	PreviewPlayerInfo = 5,
	ENR3EAvatarLight_MAX = 6
};

// Object: Enum Feature_NR3E.NR3EFSMSBindCallBackType
enum class ENR3EFSMSBindCallBackType : uint8_t
{
	None = 0,
	OnEnter = 1,
	OnExit = 2,
	OnUpdate = 3,
	NR3EFSMSBindCallBackType_MAX = 4
};

// Object: Enum Feature_NR3E.ENR3E3FSMGameState
enum class ENR3E3FSMGameState : uint8_t
{
	NONE = 0,
	Loading = 1,
	Normal = 2,
	EmergencyTask = 3,
	MeetingReady = 4,
	MeetingDiscuss = 5,
	MeetingVote = 6,
	MeetingShowVoteRes = 7,
	MeetingSettlement = 8,
	Night = 9,
	GameFinish = 100,
	ENR3E3FSMGameState_MAX = 101
};

// Object: Enum Feature_NR3E.ENR3EGameExperienceComponentFeatureBitMask
enum class ENR3EGameExperienceComponentFeatureBitMask : uint8_t
{
	NONE = 0,
	REPLICATE = 1,
	REALPLAYER = 2,
	TESTENV = 4,
	ENR3EGameExperienceComponentFeatureBitMask_MAX = 5
};

// Object: Enum Feature_NR3E.EMoeNR3EAbilityRunTimeInfoFlags
enum class EMoeNR3EAbilityRunTimeInfoFlags : uint8_t
{
	NONE = 0,
	USABLE = 2,
	EMoeNR3EAbilityRunTimeInfoFlags_MAX = 3
};

// Object: Enum Feature_NR3E.EGameplayCueExecutePolicy
enum class EGameplayCueExecutePolicy : uint8_t
{
	Auto = 0,
	Manual = 1,
	EGameplayCueExecutePolicy_MAX = 2
};

// Object: Enum Feature_NR3E.EVariantType
enum class EVariantType : uint8_t
{
	NONE = 0,
	T_FLOAT = 1,
	T_BOOL = 2,
	T_INT64 = 3,
	EVariantType_MAX = 4
};

// Object: Enum Feature_NR3E.ENR3EDecoratorType
enum class ENR3EDecoratorType : uint8_t
{
	Nearest = 0,
	Random = 1,
	DistanceSort = 2,
	RemoveAbilityActorIfHaveCharacter = 3,
	ENR3EDecoratorType_MAX = 4
};

// Object: Enum Feature_NR3E.ENR3EFilterType
enum class ENR3EFilterType : uint8_t
{
	Distance = 0,
	CylinderDistance = 1,
	ConstDistance = 2,
	IgnoreSelf = 3,
	GameplayTags = 4,
	Camp = 5,
	Vocation = 6,
	InView = 7,
	BoxComponent = 8,
	MultiDistance = 9,
	GameplayEffectFromUs = 10,
	ActiveQuitGame = 11,
	ENR3EFilterType_MAX = 12
};

// Object: Enum Feature_NR3E.ENR3EEntrySetType
enum class ENR3EEntrySetType : uint8_t
{
	Players = 0,
	Puppets = 1,
	DeathBody = 2,
	FeatureActor = 3,
	Overlapping = 4,
	AbilityResponder = 5,
	ENR3EEntrySetType_MAX = 6
};

// Object: Enum Feature_NR3E.EMoeNR3EVocationCamp
enum class EMoeNR3EVocationCamp : uint8_t
{
	NONE = 0,
	Hunter = 1,
	Prey = 2,
	Neutral = 3,
	EMoeNR3EVocationCamp_MAX = 4
};

// Object: Enum Feature_NR3E.ENR3E1GamePhase
enum class ENR3E1GamePhase : uint8_t
{
	GP_Default = 0,
	GP_Start = 1,
	GP_OpenDoor = 2,
	GP_HideEnd = 3,
	GP_SeekerCrazy = 4,
	GP_TimeEnd = 5,
	GP_MAX = 6
};

// Object: Enum Feature_NR3E.ENR3E2GamePhase
enum class ENR3E2GamePhase : uint8_t
{
	GP_Default = 0,
	GP_Start = 1,
	GP_End = 2,
	GP_MAX = 3
};

// Object: Enum Feature_NR3E.ENR3E3MeetingVoteType
enum class ENR3E3MeetingVoteType : uint8_t
{
	NONE = 0,
	AnonymousVote = 1,
	SignVote = 2,
	ENR3E3MeetingVoteType_MAX = 3
};

// Object: Enum Feature_NR3E.ENR3E3MeetingDiscussType
enum class ENR3E3MeetingDiscussType : uint8_t
{
	NONE = 0,
	SequenceDiscuss = 1,
	FreeDiscuss = 2,
	ENR3E3MeetingDiscussType_MAX = 3
};

// Object: Enum Feature_NR3E.ENR3E3ChooseType
enum class ENR3E3ChooseType : uint8_t
{
	NONE = 0,
	Include = 1,
	Exclude = 2,
	ENR3E3ChooseType_MAX = 3
};

// Object: Enum Feature_NR3E.ENR3E3ConfigType
enum class ENR3E3ConfigType : uint8_t
{
	NONE = 0,
	VocationConfig = 1,
	BrithLocationConfig = 2,
	MapConfig = 3,
	MeetingConfig = 4,
	FacilityConfig = 5,
	FacilityParamConfig = 6,
	NormalTaskConfig = 7,
	UrgentTaskConfig = 8,
	UrgentTaskParamConfig = 9,
	GameSettlementConfig = 10,
	ENR3E3ConfigType_MAX = 11
};

// Object: Enum Feature_NR3E.ENR3E3ScenePointType
enum class ENR3E3ScenePointType : uint8_t
{
	NONE = 0,
	NormalTaskPoint = 1,
	UrgentTaskPoint = 2,
	SpawnPointPoint = 3,
	SceneFacilityPoint = 4,
	ENR3E3ScenePointType_MAX = 5
};

// Object: Enum Feature_NR3E.ENR3E3DoorState
enum class ENR3E3DoorState : uint8_t
{
	Opening = 0,
	Closing = 1,
	ENR3E3DoorState_MAX = 2
};

// Object: Enum Feature_NR3E.ENR3E3EncryptDataType
enum class ENR3E3EncryptDataType : uint8_t
{
	NONE = 0,
	CampKey = 1,
	SideKey = 2,
	ENR3E3EncryptDataType_MAX = 3
};

// Object: Enum Feature_NR3E.ENR3E3EncryptRequestType
enum class ENR3E3EncryptRequestType : uint8_t
{
	NONE = 0,
	AllPlayer = 1,
	Camp = 2,
	ENR3E3EncryptRequestType_MAX = 3
};

// Package: Feature_ProjectT
// Enums: 18

// Object: Enum Feature_ProjectT.EPTMoveToResult
enum class EPTMoveToResult : uint8_t
{
	Success = 0,
	Abort = 1,
	Failure = 2,
	Blocked = 3,
	EPTMoveToResult_MAX = 4
};

// Object: Enum Feature_ProjectT.EPjtSeatOccupantType
enum class EPjtSeatOccupantType : uint8_t
{
	None = 0,
	Player = 1,
	NPC = 2,
	EPjtSeatOccupantType_MAX = 3
};

// Object: Enum Feature_ProjectT.EPjtBpFlag
enum class EPjtBpFlag : uint8_t
{
	OnMyActorBeginOverlap = 0,
	OnMyActorEndOverlap = 1,
	OnFirstCharEnterActor = 2,
	OnFirstCharLeaveActor = 3,
	OnMyTriggerBeginOverlap = 4,
	OnMyTriggerEndOverlap = 5,
	CanOperate_Client = 6,
	OnReqEnter_Server = 7,
	OnReqExit_Server = 8,
	OnReqOperate_Server = 9,
	CanEnterSeat_Server = 10,
	CanExitSeat_Server = 11,
	CharDoEnter_Server = 12,
	CharDoExit_Server = 13,
	OnCharStateChange_Server = 14,
	OnCharStateChange_Client = 15,
	OnNPCStateChange_Client = 16,
	OnOperateRes_Client = 17,
	OnCharacterReset = 18,
	OnInteractBindEnterEnd = 19,
	OnInteractBindExitEnd = 20,
	Server_OnReqOperate = 21,
	OnMulticastAction = 22,
	OnAdaptionComplete = 23,
	EPjtBpFlag_MAX = 24
};

// Object: Enum Feature_ProjectT.EPjtIaReactionProcess
enum class EPjtIaReactionProcess : uint8_t
{
	None = 0,
	Enter = 1,
	Standby = 2,
	Exit = 4,
	EPjtIaReactionProcess_MAX = 5
};

// Object: Enum Feature_ProjectT.EPjtCharBpFlag
enum class EPjtCharBpFlag : uint8_t
{
	SkipInteractComp = 0,
	GetInteractComponentFromUid = 1,
	IsPlayAnim = 2,
	CanEnterSeat = 3,
	CanOperate = 4,
	Log = 5,
	IsOnWater = 6,
	EPjtCharBpFlag_MAX = 7
};

// Object: Enum Feature_ProjectT.EProjectTDialogType
enum class EProjectTDialogType : uint8_t
{
	EDT_Dialog = 0,
	EDT_Hint = 1,
	EDT_MAX = 2
};

// Object: Enum Feature_ProjectT.EPjtIaResCode
enum class EPjtIaResCode : uint8_t
{
	Success = 0,
	IsBusy = 1,
	NoActor = 2,
	Other = 3,
	EPjtIaResCode_MAX = 4
};

// Object: Enum Feature_ProjectT.EPjtIaState
enum class EPjtIaState : uint8_t
{
	None = 0,
	Entering = 1,
	Gaming = 2,
	Exiting = 3,
	Creating = 4,
	Destroy = 5,
	Carrying = 6,
	EPjtIaState_MAX = 7
};

// Object: Enum Feature_ProjectT.EPjtIaAniTargetType
enum class EPjtIaAniTargetType : uint8_t
{
	NoTarget = 0,
	Point = 1,
	RelativeTrans = 2,
	EPjtIaAniTargetType_MAX = 3
};

// Object: Enum Feature_ProjectT.EPjtIaOperation
enum class EPjtIaOperation : uint8_t
{
	Enter = 0,
	Exit = 1,
	Other = 2,
	EPjtIaOperation_MAX = 3
};

// Object: Enum Feature_ProjectT.EProjectTIsmTerrainBpFlag
enum class EProjectTIsmTerrainBpFlag : uint8_t
{
	RecycleComp = 0,
	Log = 1,
	LogWarning = 2,
	LogUpdate = 3,
	GetTimePrecise = 4,
	AddIsmInst = 5,
	AddProxyInst = 6,
	EProjectTIsmTerrainBpFlag_MAX = 7
};

// Object: Enum Feature_ProjectT.EPTNPCFlyFollowFacingMode
enum class EPTNPCFlyFollowFacingMode : uint8_t
{
	None = 0,
	MoveDirection = 1,
	TargetActor = 2,
	TargetRotation = 3,
	EPTNPCFlyFollowFacingMode_MAX = 4
};

// Object: Enum Feature_ProjectT.ERippleTileTypeRT
enum class ERippleTileTypeRT : uint8_t
{
	OC_FullCover = 0,
	OC_FadeLeft = 1,
	OC_FadeRight = 2,
	Edge_FullCover = 3,
	Edge_FadeLeft = 4,
	Edge_FadeRight = 5,
	IC_FullCover = 6,
	RippleMAX = 7,
	ERippleTileTypeRT_MAX = 8
};

// Object: Enum Feature_ProjectT.ERoadTileTypeRT
enum class ERoadTileTypeRT : uint8_t
{
	Fill = 0,
	InnerCorner = 1,
	OuterCorner = 2,
	EdgeH = 3,
	EdgeV = 4,
	MAX = 5
};

// Object: Enum Feature_ProjectT.EPTPathFindingBackend
enum class EPTPathFindingBackend : uint8_t
{
	Grid = 0,
	NavMesh = 1,
	EPTPathFindingBackend_MAX = 2
};

// Object: Enum Feature_ProjectT.EPTIndicatorBpFlag
enum class EPTIndicatorBpFlag : uint8_t
{
	UpdateIndicator = 0,
	ShowRootWidget = 1,
	UpdateWidget = 2,
	EPTIndicatorBpFlag_MAX = 3
};

// Object: Enum Feature_ProjectT.EPTMoveToStatus
enum class EPTMoveToStatus : uint8_t
{
	None = 0,
	Paused = 1,
	Moving = 2,
	EPTMoveToStatus_MAX = 3
};

// Object: Enum Feature_ProjectT.EPjtYuanbaoCarryState
enum class EPjtYuanbaoCarryState : uint8_t
{
	Idle = 0,
	Lifting = 1,
	Carrying = 2,
	EPjtYuanbaoCarryState_MAX = 3
};

// Package: Feature_TYC
// Enums: 54

// Object: Enum Feature_TYC.EOMDBuildRaycastType
enum class EOMDBuildRaycastType : uint8_t
{
	Sweep = 0,
	LineTrace = 1,
	EOMDBuildRaycastType_MAX = 2
};

// Object: Enum Feature_TYC.EOMDPreviewStateType
enum class EOMDPreviewStateType : uint8_t
{
	Default = 0,
	NoTrapField = 1,
	TrapOccupied = 2,
	TrapFieldGridInvalid = 3,
	LocationTypeWrong = 4,
	TrapFieldTypeWrong = 5,
	Preview = 99,
	EOMDPreviewStateType_MAX = 100
};

// Object: Enum Feature_TYC.EOMDBuildTrapIndicatorColor
enum class EOMDBuildTrapIndicatorColor : uint8_t
{
	Blue = 0,
	Red = 1,
	None = 2,
	EOMDBuildTrapIndicatorColor_MAX = 3
};

// Object: Enum Feature_TYC.EOMDDamagePlayType
enum class EOMDDamagePlayType : uint8_t
{
	PlayNone = 0,
	PlayDelay = 1,
	PlayStart = 2,
	Playing = 3,
	PlayFinished = 4,
	EOMDDamagePlayType_MAX = 5
};

// Object: Enum Feature_TYC.EOMDDamageType
enum class EOMDDamageType : uint8_t
{
	DamageNone = 0,
	DamageImmunity = 1,
	DamageHeadShot = 2,
	DamageDefault = 3,
	DamageCombo = 4,
	DamageGeneral = 5,
	EOMDDamageType_MAX = 6
};

// Object: Enum Feature_TYC.EOMDScalObjectType
enum class EOMDScalObjectType : uint8_t
{
	Default = 0,
	TextDamage = 1,
	HeadShotIcon = 2,
	ComboIcon = 3,
	EOMDScalObjectType_MAX = 4
};

// Object: Enum Feature_TYC.EOMDConditionType
enum class EOMDConditionType : uint8_t
{
	OMDCT_DEFAULT = 0,
	OMDCT_ALWAYS_TRUE = 1,
	OMDCT_ALWAYS_FALSE = 2,
	OMDCT_KILL = 3,
	OMDCT_HURT = 4,
	OMDCT_HEAD_HURT = 5,
	OMDCT_TRIGGER = 6,
	OMDCT_CUR_LEVEL_REACH = 7,
	OMDCT_USE_CROWN = 8,
	OMDCT_BUILD_TRAP = 9,
	OMDCT_HURT_TIMES = 10,
	OMDCT_DEAD = 11,
	OMDCT_SELL_TRAP = 12,
	OMDCT_BUILD_TRAP_ACTION = 13,
	OMDCT_ENTER_ELECTRIC_DOOR = 14,
	OMDCT_HEAD_HURT_TIMES = 15,
	OMDCT_MAX = 16
};

// Object: Enum Feature_TYC.EOMDStatObjType
enum class EOMDStatObjType : uint8_t
{
	OMDSOT_DEFAULT = 0,
	OMDSOT_TRAP = 1,
	OMDSOT_WEAPON_PRIMARY = 2,
	OMDSOT_WEAPON_SECONDARY = 3,
	OMDSOT_WEAPON_ALL = 4,
	OMDSOT_PLAYER = 5,
	OMDSOT_SIDE = 6,
	OMDSOT_MONSTER = 7,
	OMDSOT_MAX = 8
};

// Object: Enum Feature_TYC.EDamageTargetType
enum class EDamageTargetType : uint8_t
{
	Orcs = 0,
	Players = 1,
	AllActors = 2,
	EDamageTargetType_MAX = 3
};

// Object: Enum Feature_TYC.EOMDPropStateType
enum class EOMDPropStateType : uint8_t
{
	Default = 0,
	Unlocked = 1,
	Equipped = 2,
	RedDotDisplaying = 3,
	RedDotClicked = 4,
	EOMDPropStateType_MAX = 5
};

// Object: Enum Feature_TYC.OMDWeaponType
enum class EOMDWeaponType : uint8_t
{
	Unknown = 0,
	OWP_MagicBow = 1,
	OWP_Hammer = 2,
	OWP_DominanceWand = 3,
	OWP_Buss = 4,
	OWP_Sword = 5,
	OWP_CrossBow = 6,
	OMDWeaponType_MAX = 7
};

// Object: Enum Feature_TYC.EWeaponHandType
enum class EWeaponHandType : uint8_t
{
	LeftHandWeapon = 0,
	RightHandWeapon = 1,
	EWeaponHandType_MAX = 2
};

// Object: Enum Feature_TYC.EOMDLevelType
enum class EOMDLevelType : uint8_t
{
	OMDLT_LOBBY = 0,
	OMDLT_BATTLE = 1,
	OMDLT_MOE_LOBBY = 2,
	OMDLT_CHALLENGE = 3,
	OMDLT_MAX = 4
};

// Object: Enum Feature_TYC.ETYCOMDLevelDifficultyType
enum class ETYCOMDLevelDifficultyType : uint8_t
{
	NOLDT_Default = 0,
	NOLDT_NORMAL = 1,
	NOLDT_DIFFICULTY = 2,
	NOLDT_ENDLESS = 3,
	NOLDT_MAX = 4
};

// Object: Enum Feature_TYC.EOMDLevelStateEnum
enum class EOMDLevelStateEnum : uint8_t
{
	OMDLSE_NONE = 0,
	OMDLSE_UNFINISH = 1,
	OMDLSE_FINISH = 2,
	OMDLSE_MAX = 3
};

// Object: Enum Feature_TYC.EFaceDirection
enum class EFaceDirection : uint8_t
{
	InValid = 0,
	FaceUp = 1,
	FaceDown = 2,
	FaceWest = 3,
	FaceNorth = 4,
	FaceEast = 5,
	FaceSouth = 6,
	EFaceDirection_MAX = 7
};

// Object: Enum Feature_TYC.EFieldTrapType
enum class EFieldTrapType : uint8_t
{
	InValid = 0,
	Normal = 1,
	NormalAndWarField = 2,
	WarFieldOnly = 3,
	EFieldTrapType_MAX = 4
};

// Object: Enum Feature_TYC.ETYCMonsterWaveEnum
enum class ETYCMonsterWaveEnum : uint8_t
{
	Null = 1,
	Relax = 2,
	Battle_CountDown = 3,
	Battle = 4,
	Explore = 5,
	EndLess = 6,
	PvP = 7,
	Pass = 8,
	Fail = 9,
	Preview = 10,
	Suspend = 11,
	End = 99,
	ETYCMonsterWaveEnum_MAX = 100
};

// Object: Enum Feature_TYC.ETYCCharacterEnum
enum class ETYCCharacterEnum : uint8_t
{
	Null = 0,
	Player = 1,
	Monster = 2,
	Tower = 3,
	Vehicle = 4,
	Drone = 5,
	Build = 6,
	ETYCCharacterEnum_MAX = 7
};

// Object: Enum Feature_TYC.ETYCCrowdBuffIndex
enum class ETYCCrowdBuffIndex : uint8_t
{
	OMD_IceBuff = 50,
	OMD_FireBuff = 51,
	OMD_AoshuBuff = 52,
	OMD_GoldBuff = 53,
	OMD_LightBuff = 54,
	OMD_SuanyeBuff = 55,
	OMD_ChihuanBuff = 56,
	OMD_StoneBuff = 57,
	OMD_bHit = 58,
	OMD_MAX = 59
};

// Object: Enum Feature_TYC.ETYCBuffType
enum class ETYCBuffType : uint8_t
{
	None = 0,
	ChangeScale = 1,
	ChangeWeaponDamage = 2,
	ChangeAttackSpeed = 3,
	RootDebuff = 4,
	ChangeInjuryRatio = 5,
	BeTaunting = 6,
	ChangeSpeed = 7,
	TakeDamage = 8,
	AddSkill = 9,
	Recover = 10,
	OverObstacle = 11,
	ChangeAttr = 12,
	BuffAction = 13,
	HitFly = 14,
	DropItem = 15,
	ETYCBuffType_MAX = 16
};

// Object: Enum Feature_TYC.ETYCBuffTargetType
enum class ETYCBuffTargetType : uint8_t
{
	TBTT_Normal = 0,
	TBTT_Elite = 1,
	TBTT_Boss = 2,
	TBTT_Player = 3,
	TBTT_FlyMonster = 4,
	TBTT_MAX = 5
};

// Object: Enum Feature_TYC.ETYCSkillTargetType
enum class ETYCSkillTargetType : uint8_t
{
	TSTT_Any = 0,
	TSTT_Player = 1,
	TSTT_Monster = 2,
	TSTT_Tower = 3,
	TSTT_ObstacleTrap = 4,
	TSTT_TauntTrap = 5,
	TSTT_NormalTrap = 6,
	TSTT_Caster = 7,
	TSTT_Position = 8,
	TSTT_MAX = 9
};

// Object: Enum Feature_TYC.ETYCCustomMovementMode
enum class ETYCCustomMovementMode : uint8_t
{
	Custom_ClimbLadder = 0,
	Custom_DrivingTurret = 1,
	Custom_MAX = 2
};

// Object: Enum Feature_TYC.EKnockOffDieType
enum class EKnockOffDieType : uint8_t
{
	KODT_None = 0,
	KODT_KnockOffUp = 1,
	KODT_KnockOffWithForward = 2,
	KODT_MAX = 3
};

// Object: Enum Feature_TYC.ETYCMovementMode
enum class ETYCMovementMode : uint8_t
{
	TYC_MOVE_None = 0,
	TYC_MOVE_PurePhysics = 1,
	TYC_MOVE_AfterPhysics = 2,
	TYC_MOVE_NavWalking = 3,
	TYC_MOVE_FlyWalking = 4,
	TYC_MOVE_UGCWalking = 5,
	TYC_MOVE_Falling = 6,
	TYC_MOVE_JumpingToAirTargetPos = 7,
	TYC_MOVE_PushWithRVO = 8,
	TYC_MOVE_Launch = 9,
	TYC_MOVE_Die = 10,
	TYC_MOVE_KnockOffDying = 11,
	TYC_MOVE_KnockOffDieFalling = 12,
	TYC_MOVE_KnockOffDieEnd = 13,
	TYC_MOVE_FallingDie = 14,
	TYC_MOVE_SuctionedUp = 15,
	TYC_MOVE_SuctionedDown = 16,
	TYC_MOVE_SuctionedDownEnd = 17,
	TYC_MOVE_MAX = 18
};

// Object: Enum Feature_TYC.EAgentTreeType
enum class EAgentTreeType : uint16_t
{
	ATT_Null = 0,
	ATT_InValid = 1,
	ATT_AboutMonster = 2,
	ATT_ObstacleTrap = 4,
	ATT_TauntTrap = 8,
	ATT_NormalTrap = 16,
	ATT_Player = 32,
	ATT_AboutRedSideMonster = 64,
	ATT_AboutNeutralityMonster = 128,
	ATT_ALL = 255,
	ATT_MAX = 256
};

// Object: Enum Feature_TYC.EAgentType
enum class EAgentType : uint8_t
{
	AT_Null = 0,
	AT_InValid = 1,
	AT_Monster = 2,
	AT_GuideGhostMonster = 3,
	AT_ObstacleTrap = 4,
	AT_TauntTrap = 5,
	AT_NormalTrap = 6,
	AT_Player = 7,
	AT_RedSideMonster = 8,
	AT_NeutralityMonster = 9,
	AT_ALL = 10,
	AT_MAX = 11
};

// Object: Enum Feature_TYC.EOMDMonsterKillByErrorReason
enum class EOMDMonsterKillByErrorReason : uint8_t
{
	OMKBER_None = 0,
	OMKBER_AIErrorCheck = 1,
	OMKBER_NavMeshNotFound = 2,
	OMKBER_LocationTooLow = 3,
	OMKBER_PhysicsUnmoving = 4,
	OMKBER_TooFarToTargetPath = 5,
	OMKBER_MAX = 6
};

// Object: Enum Feature_TYC.EOMDMonsterType
enum class EOMDMonsterType : uint8_t
{
	OMT_None = 0,
	OMT_Normal = 1,
	OMT_Elite = 2,
	OMT_YellowDiamond = 3,
	OMT_Triangle = 4,
	OMT_Boss = 5,
	OMT_MAX = 6
};

// Object: Enum Feature_TYC.ETYCMonsterSoundType
enum class ETYCMonsterSoundType : uint8_t
{
	Move = 0,
	BeHit = 1,
	Dead = 2,
	ETYCMonsterSoundType_MAX = 3
};

// Object: Enum Feature_TYC.ETYCPathFindType
enum class ETYCPathFindType : uint8_t
{
	Default = 0,
	IgnoreObstacles = 1,
	ETYCPathFindType_MAX = 2
};

// Object: Enum Feature_TYC.ETYCSkillType
enum class ETYCSkillType : uint8_t
{
	TST_Melee = 0,
	TST_Bullet = 1,
	TST_Call = 2,
	TST_Trigger = 3,
	TST_Buff = 4,
	TST_CallMonster = 5,
	TST_Action = 6,
	TST_Bounce = 7,
	TST_GatherMonster = 8,
	TST_SelfBoom = 9,
	TST_Rush = 10,
	TST_TYCBounce = 11,
	TST_MAX = 12
};

// Object: Enum Feature_TYC.ETYCOMDTrapPreviewType
enum class ETYCOMDTrapPreviewType : uint8_t
{
	EOMDTrapPreviewNormal = 0,
	EOMDTrapSawBlade = 1,
	ETYCOMDTrapPreviewType_MAX = 2
};

// Object: Enum Feature_TYC.ETYCMaterialChangeType
enum class ETYCMaterialChangeType : uint8_t
{
	EmaterialChangeCoolDown = 0,
	EmaterialChangeTrigger = 1,
	EmaterialChangeBeforeFire = 2,
	ETYCMaterialChangeType_MAX = 3
};

// Object: Enum Feature_TYC.ETYCMultiCDType
enum class ETYCMultiCDType : uint8_t
{
	EMultiCDNone = 0,
	EMultiCDBefore = 1,
	EMultiCDMiddle = 2,
	EMultiCDAfter = 3,
	ETYCMultiCDType_MAX = 4
};

// Object: Enum Feature_TYC.ETYCTriggerLocationType
enum class ETYCTriggerLocationType : uint8_t
{
	Unknown = 0,
	Floor = 1,
	Wall = 2,
	Ceiling = 3,
	FloorAndWall = 4,
	FloorAndCeiling = 5,
	WallAndCeiling = 6,
	AllPlace = 7,
	ETYCTriggerLocationType_MAX = 8
};

// Object: Enum Feature_TYC.ETYCTriggerAbilityType
enum class ETYCTriggerAbilityType : uint8_t
{
	None = 0,
	Spring = 1,
	Pull = 2,
	Taunt = 3,
	ETYCTriggerAbilityType_MAX = 4
};

// Object: Enum Feature_TYC.ETYCTriggerAnimType
enum class ETYCTriggerAnimType : uint8_t
{
	Unknown = 0,
	Idle = 1,
	Trigger = 2,
	ETYCTriggerAnimType_MAX = 3
};

// Object: Enum Feature_TYC.ETYCTriggerFireType
enum class ETYCTriggerFireType : uint8_t
{
	ByTime = 0,
	ByTarget = 1,
	ETYCTriggerFireType_MAX = 2
};

// Object: Enum Feature_TYC.ETYCTriggerCategory
enum class ETYCTriggerCategory : uint8_t
{
	Normal = 0,
	Trap = 1,
	OnlyTrap = 2,
	ETYCTriggerCategory_MAX = 3
};

// Object: Enum Feature_TYC.ETYCRelationType
enum class ETYCRelationType : uint8_t
{
	Unknown = 0,
	Self = 1,
	Other = 2,
	ETYCRelationType_MAX = 3
};

// Object: Enum Feature_TYC.TYCTriggerCheckType
enum class ETYCTriggerCheckType : uint8_t
{
	Unknown = 0,
	InOutCheck = 1,
	DamageCheck = 2,
	All = 3,
	TYCTriggerCheckType_MAX = 4
};

// Object: Enum Feature_TYC.TYCShapeType
enum class ETYCShapeType : uint8_t
{
	Unknown = 0,
	Sector = 1,
	Circle = 2,
	Rectangle = 3,
	Capsule = 4,
	Cube = 5,
	TYCShapeType_MAX = 6
};

// Object: Enum Feature_TYC.ETYCTrapTransitionType
enum class ETYCTrapTransitionType : uint8_t
{
	None = 0,
	CDTimeOut = 1,
	ETYCTrapTransitionType_MAX = 2
};

// Object: Enum Feature_TYC.ETYCTriggerStateType
enum class ETYCTriggerStateType : uint8_t
{
	None = 0,
	Init = 1,
	CoolDown = 2,
	Alert = 3,
	Trigger = 4,
	Dead = 5,
	NearestAlert = 6,
	End = 7,
	TriggerSustain = 8,
	BeforeTrigger = 9,
	TriggerMultiple = 10,
	TriggerCharge = 11,
	AfterTrigger = 12,
	AfterChargeTrigger = 13,
	MultiCD = 14,
	BeforeFire = 15,
	Reload = 16,
	Gravity = 17,
	TriggerSustainLive = 18,
	AfterBuildCoolDown = 19,
	Recover = 20,
	Dummy = 21,
	ETYCTriggerStateType_MAX = 22
};

// Object: Enum Feature_TYC.ETYCUGCTowerType
enum class ETYCUGCTowerType : uint8_t
{
	GunTower = 0,
	FireTower = 1,
	RPGTower = 2,
	IceTower = 3,
	SpringTrap = 4,
	SpikeTrap = 5,
	ElectricTrap = 6,
	ScarecrowTrap = 7,
	ETYCUGCTowerType_MAX = 8
};

// Object: Enum Feature_TYC.ETYCAttrEnum
enum class ETYCAttrEnum : uint8_t
{
	None = 0,
	TYC_Fire = 1,
	TYC_Electric = 2,
	TYC_Ice = 3,
	OMD_Phy = 4,
	OMD_Fire = 5,
	OMD_Electric = 6,
	OMD_Ice = 7,
	OMD_Magic = 8,
	ETYCAttrEnum_MAX = 9
};

// Object: Enum Feature_TYC.ETYCMonsterType
enum class ETYCMonsterType : uint8_t
{
	Common = 0,
	Elite = 1,
	Bosss = 2,
	ETYCMonsterType_MAX = 3
};

// Object: Enum Feature_TYC.ETYCDeviceEffect
enum class ETYCDeviceEffect : uint8_t
{
	None = 0,
	DroneSKM = 1,
	MonsterTrailingGhost = 2,
	OpenHighCullDisVolume = 3,
	FlagSKM = 4,
	BattleAudio = 5,
	BattleEffect = 6,
	WeaponCameraShake = 7,
	ShowHitDecal = 8,
	PlayWeaponSoundGap = 9,
	OpenLight = 10,
	TrapEffect = 11,
	WeaponEffect = 12,
	ETYCDeviceEffect_MAX = 13
};

// Object: Enum Feature_TYC.EAIBTTargetType
enum class EAIBTTargetType : uint8_t
{
	None = 0,
	Platform = 1,
	Weapon = 2,
	TowerOrTrap = 3,
	Monster = 4,
	Monitor = 5,
	PVPArea = 6,
	PickUpProp = 7,
	Scientist = 8,
	WeaponLevelUp = 9,
	EAIBTTargetType_MAX = 10
};

// Object: Enum Feature_TYC.EMapOperationType
enum class EMapOperationType : uint8_t
{
	None = 0,
	Get = 1,
	Add = 2,
	Delete = 3,
	Update = 4,
	UnRegist = 5,
	EMapOperationType_MAX = 6
};

// Object: Enum Feature_TYC.ETYCPreviewConditionType
enum class ETYCPreviewConditionType : uint8_t
{
	None = 0,
	ReincarnationTimes = 1,
	BuildProgress = 2,
	DependBuilding = 3,
	ETYCPreviewConditionType_MAX = 4
};

// Object: Enum Feature_TYC.ETYCBuildItemType
enum class ETYCBuildItemType : uint8_t
{
	None = 0,
	Scientist = 1,
	Bank = 2,
	Weapon = 3,
	Armor = 4,
	Platform = 5,
	ElectricDoor = 6,
	Drone = 7,
	Tower = 8,
	Reincarnation = 9,
	GravityArea = 10,
	PlayerBuildingUI = 11,
	AttackBuilding = 12,
	DrivingTurret = 13,
	CarrierBuilding = 14,
	Monitor = 15,
	EditWeapon = 16,
	HpScreen = 17,
	WeaponEffect = 18,
	FakeLight = 19,
	ETYCBuildItemType_MAX = 20
};

// Package: Feature_FPS
// Enums: 1

// Object: Enum Feature_FPS.EVolumeShape
enum class EVolumeShape : uint8_t
{
	Cube = 0,
	Sphere = 1,
	EVolumeShape_MAX = 2
};

// Package: Feature_UGC
// Enums: 82

// Object: Enum Feature_UGC.EUGCBindingMode
enum class EUGCBindingMode : uint8_t
{
	Auto = 0,
	Static = 1,
	Dynamic = 2,
	EUGCBindingMode_MAX = 3
};

// Object: Enum Feature_UGC.EUGCBindingState
enum class EUGCBindingState : uint8_t
{
	None = 0,
	Ready = 1,
	Pending = 2,
	Start = 3,
	Loading = 4,
	Finished = 5,
	EUGCBindingState_MAX = 6
};

// Object: Enum Feature_UGC.EUGCVisibilityType
enum class EUGCVisibilityType : uint8_t
{
	Hide = 0,
	Show = 1,
	Auto = 2,
	EUGCVisibilityType_MAX = 3
};

// Object: Enum Feature_UGC.EParameterType
enum class EParameterType : uint8_t
{
	EPT_Int32 = 0,
	EPT_Float = 1,
	EPT_Bool = 2,
	EPT_String = 3,
	EPT_Object = 4,
	EPT_MAX = 5
};

// Object: Enum Feature_UGC.ENotifyType
enum class ENotifyType : uint8_t
{
	ENT_Invalid = 0,
	ENT_Notify = 1,
	ENT_NotifyBegin = 2,
	ENT_NotifyTick = 3,
	ENT_NotifyEnd = 4,
	ENT_MAX = 5
};

// Object: Enum Feature_UGC.EDynamicParamWeaponType
enum class EDynamicParamWeaponType : uint8_t
{
	None = 0,
	CustomWeapon = 1,
	PunchWeapon = 2,
	EDynamicParamWeaponType_MAX = 3
};

// Object: Enum Feature_UGC.EUGCGameCharCameraType
enum class EUGCGameCharCameraType : uint8_t
{
	None = 0,
	TP_FREE = 1,
	TP_FOLLOW = 2,
	FP = 3,
	DEATH = 4,
	EUGCGameCharCameraType_MAX = 5
};

// Object: Enum Feature_UGC.ESkillButtonState
enum class ESkillButtonState : uint8_t
{
	Press = 0,
	Release = 1,
	ESkillButtonState_MAX = 2
};

// Object: Enum Feature_UGC.ESkillImpactDir
enum class ESkillImpactDir : uint8_t
{
	None = 0,
	Default = 1,
	Front = 2,
	Back = 3,
	Left = 4,
	Right = 5,
	ESkillImpactDir_MAX = 6
};

// Object: Enum Feature_UGC.ESkillImpactType
enum class ESkillImpactType : uint8_t
{
	None = 0,
	BlastAir = 1,
	BlastLand = 2,
	Backlash = 3,
	Stun = 4,
	Numb = 5,
	ESkillImpactType_MAX = 6
};

// Object: Enum Feature_UGC.ESkillInfoFaceType
enum class ESkillInfoFaceType : uint8_t
{
	None = 0,
	Target = 1,
	Indicator = 2,
	Camera = 3,
	TargetOrMoveInput = 4,
	TargetOrCamera = 5,
	MoveInput = 6,
	CustomizedTargetOrCamera = 7,
	ESkillInfoFaceType_MAX = 8
};

// Object: Enum Feature_UGC.EAimingPointTargetIdentity
enum class EAimingPointTargetIdentity : uint8_t
{
	None = 0,
	Enemy = 1,
	All = 2,
	EAimingPointTargetIdentity_MAX = 3
};

// Object: Enum Feature_UGC.ECharacterRoleTypeForEffectLOD
enum class ECharacterRoleTypeForEffectLOD : uint8_t
{
	None = 0,
	Player = 1,
	Boss = 2,
	Elite = 3,
	Normal = 4,
	ECharacterRoleTypeForEffectLOD_MAX = 5
};

// Object: Enum Feature_UGC.EBattleEffectLODScoreSkillType
enum class EBattleEffectLODScoreSkillType : uint8_t
{
	None = 0,
	Melee = 1,
	Projectiles = 2,
	EBattleEffectLODScoreSkillType_MAX = 3
};

// Object: Enum Feature_UGC.EBattleEffectLODScorePlayerRoleType
enum class EBattleEffectLODScorePlayerRoleType : uint8_t
{
	NotPlayer = 0,
	LocalPlayer = 1,
	OtherPlayer = 2,
	EBattleEffectLODScorePlayerRoleType_MAX = 3
};

// Object: Enum Feature_UGC.EBattleEffectLOD
enum class EBattleEffectLOD : uint8_t
{
	None = 0,
	LOD0 = 1,
	LOD1 = 2,
	LOD2 = 3,
	NotShow = 4,
	EBattleEffectLOD_MAX = 5
};

// Object: Enum Feature_UGC.EOperationControlMode
enum class EOperationControlMode : uint8_t
{
	None = 0,
	TPS = 1,
	ACT = 2,
	Interact = 3,
	EOperationControlMode_MAX = 4
};

// Object: Enum Feature_UGC.EBehaviorEndReason
enum class EBehaviorEndReason : uint8_t
{
	Finished = 0,
	NewAnimation = 1,
	MoveAction = 2,
	SkillLogic = 3,
	GrapplingHook = 4,
	ParkourActionFinish = 5,
	Buff = 6,
	Respawn = 7,
	POI = 8,
	Dialogue = 9,
	LogicState = 10,
	SwimLogic = 11,
	InterruptedByParkour = 12,
	ParkourInterruptedByOther = 13,
	InterruptedBySameGroupAnim = 14,
	EBehaviorEndReason_MAX = 15
};

// Object: Enum Feature_UGC.ESkillInterruptReason
enum class ESkillInterruptReason : uint8_t
{
	None = 0,
	Finished = 1,
	Interrupt = 2,
	CostFailed = 3,
	Blueprint = 4,
	SkillOutBlocked = 5,
	InterruptCategory = 6,
	Behavior = 7,
	Move = 8,
	StartNewIndicatorSkill = 9,
	BPBuff = 10,
	BPSkill = 11,
	Death = 12,
	BehaviorFinishCondition = 13,
	Parkour = 14,
	Grappling = 15,
	ChangeFightStyle = 16,
	PreClient = 17,
	PreClientFix = 18,
	BPLogicState = 19,
	Jump = 20,
	ESkillInterruptReason_MAX = 21
};

// Object: Enum Feature_UGC.ESkillSelectType
enum class ESkillSelectType : uint8_t
{
	Positive = 0,
	Negative = 1,
	TwoSide = 2,
	ESkillSelectType_MAX = 3
};

// Object: Enum Feature_UGC.ESkillResult
enum class ESkillResult : uint8_t
{
	Successed = 0,
	ParamInvalid = 1,
	SpellActionForbid = 2,
	GetDefaultObjectFailed = 3,
	NeedStatusFailed = 4,
	NewSkillObjectFailed = 5,
	Interrupted = 6,
	CheckCDFailed = 7,
	ActorSkillComponentNull = 8,
	CheckAuthorityFailed = 9,
	CheckStartTickNumFailed = 10,
	CheckCastCountFailed = 11,
	CheckStartCastListNumFailed = 12,
	ActorSkillSlotComponentNull = 13,
	ChangeFightStyle = 14,
	ESkillResult_MAX = 15
};

// Object: Enum Feature_UGC.ESkillSlotType
enum class ESkillSlotType : uint8_t
{
	None = 0,
	CommonAttack = 1,
	Dash = 2,
	Ability1 = 3,
	Ability2 = 4,
	Ability3 = 5,
	Ability4 = 6,
	Ultimate = 7,
	HolsterWeapon = 8,
	AbilityQTE = 9,
	ESkillSlotType_MAX = 10
};

// Object: Enum Feature_UGC.EUS_RPCType
enum class EUS_RPCType : uint8_t
{
	ToServer_Reliable = 0,
	ToClient_Reliable = 1,
	Broadcast_Reliable = 2,
	ToServer_Unreliable = 3,
	ToClient_Unreliable = 4,
	Broadcast_Unreliable = 5,
	EUS_MAX = 6
};

// Object: Enum Feature_UGC.EMoeCharacterAnimType
enum class EMoeCharacterAnimType : uint8_t
{
	EMoeCharAnim_Idle = 0,
	EMoeCharAnim_NoWeaponMoveBlendSpace = 1,
	EMoeCharAnim_ASIdle = 2,
	EMoeCharAnim_MoveBlendSpace = 3,
	EMoeCharAnim_ASFireStart = 4,
	EMoeCharAnim_ASFireLoop = 5,
	EMoeCharAnim_ASFireEnd = 6,
	EMoeCharAnim_ASChangeClip = 7,
	EMoeCharAnim_ASChangeClip_Empty = 8,
	EMoeCharAnim_ASChangeClipLoop = 9,
	EMoeCharAnim_ASChangeClipEnd = 10,
	EMoeCharAnim_ASGetWeapon = 11,
	EMoeCharAnim_HitBlendSpace = 12,
	EMoeCharAnim_AOAimOffset = 13,
	EMoeCharAnim_ASOperatingBomb = 14,
	EMoeCharAnim_ASIdlePose = 15,
	EMoeCharAnim_ASIdleness = 16,
	EMoeCharAnim_ASIronSightsIdle = 17,
	EMoeCharAnim_ASIronSightsFiring = 18,
	EMoeCharAnim_ASIronSightsFireStart = 19,
	EMoeCharAnim_ASIronSightsFireEnd = 20,
	EMoeCharAnim_ASIronSightsOn = 21,
	EMoeCharAnim_Max = 22
};

// Object: Enum Feature_UGC.ELoadedAnimCacheType
enum class ELoadedAnimCacheType : uint8_t
{
	ELoadedAnimCacheType_CharacterTPP = 1,
	ELoadedAnimCacheType_CharacterFPP = 2,
	ELoadedAnimCacheType_SkeletalWeaponTPP = 3,
	ELoadedAnimCacheType_SkeletalWeaponFPP = 4,
	ELoadedAnimCacheType_WeaponCameraShake = 5,
	ELoadedAnimCacheType_MAX = 6
};

// Object: Enum Feature_UGC.ERecoilState
enum class ERecoilState : uint8_t
{
	Idle = 0,
	Recoiling = 1,
	Recovering = 2,
	ERecoilState_MAX = 3
};

// Object: Enum Feature_UGC.EUGCPawnType
enum class EUGCPawnType : uint8_t
{
	Character = 0,
	Spectator = 1,
	Vehicle = 2,
	UGC_Editor = 3,
	EUGCPawnType_MAX = 4
};

// Object: Enum Feature_UGC.EUGCPlayModeType
enum class EUGCPlayModeType : uint8_t
{
	FP = 0,
	TP_Follow = 1,
	TP_Free = 2,
	EUGCPlayModeType_MAX = 3
};

// Object: Enum Feature_UGC.EUGCControlType
enum class EUGCControlType : uint8_t
{
	RUN = 0,
	SHOOT = 1,
	EUGCControlType_MAX = 2
};

// Object: Enum Feature_UGC.EUGCAIPatrolPathType
enum class EUGCAIPatrolPathType : uint8_t
{
	Once = 0,
	Circle = 1,
	BackAndForth = 2,
	EUGCAIPatrolPathType_MAX = 3
};

// Object: Enum Feature_UGC.EUGCAISwitcherTargetType
enum class EUGCAISwitcherTargetType : uint8_t
{
	None = 0,
	AllNPC = 1,
	SpecifiedNPC = 2,
	EUGCAISwitcherTargetType_MAX = 3
};

// Object: Enum Feature_UGC.EUGCAIBehaviorSwitcherType
enum class EUGCAIBehaviorSwitcherType : uint8_t
{
	ToNone = 0,
	ToFollowPlayer = 1,
	ToImitation = 2,
	ToBackToBornPosition = 3,
	ToPatrolPath = 4,
	MoveToTarget = 5,
	ResetDefaultState = 6,
	SwitchAnimation = 7,
	BehaviorGuard = 8,
	EUGCAIBehaviorSwitcherType_MAX = 9
};

// Object: Enum Feature_UGC.EUGCAIBehaviorSwitcherActivationType
enum class EUGCAIBehaviorSwitcherActivationType : uint8_t
{
	Touch = 0,
	Signal = 1,
	EUGCAIBehaviorSwitcherActivationType_MAX = 2
};

// Object: Enum Feature_UGC.EUGCBlackboardOpType
enum class EUGCBlackboardOpType : uint8_t
{
	Addition = 0,
	Subtraction = 1,
	Multiplication = 2,
	Division = 3,
	EUGCBlackboardOpType_MAX = 4
};

// Object: Enum Feature_UGC.EUGCAIState
enum class EUGCAIState : uint16_t
{
	None = 0,
	FollowPlayer = 1,
	Imitation = 2,
	BackToBornPosition = 3,
	PatrolPath = 4,
	MoveToTarget = 5,
	Battle = 6,
	NotInAI = 255,
	EUGCAIState_MAX = 256
};

// Object: Enum Feature_UGC.EUGCAIBTOperator
enum class EUGCAIBTOperator : uint8_t
{
	Bigger = 0,
	Lesser = 1,
	Equal = 2,
	EUGCAIBTOperator_MAX = 3
};

// Object: Enum Feature_UGC.EUGCAIControlState
enum class EUGCAIControlState : uint8_t
{
	Normal = 0,
	Imitation = 1,
	TargetPointMove = 2,
	TargetYawMove = 3,
	EUGCAIControlState_MAX = 4
};

// Object: Enum Feature_UGC.EUGCAIPropUseState
enum class EUGCAIPropUseState : uint8_t
{
	Normal = 0,
	Imitation = 1,
	EUGCAIPropUseState_MAX = 2
};

// Object: Enum Feature_UGC.EUGCAvatarType
enum class EUGCAvatarType : uint8_t
{
	Suit = 0,
	Top = 1,
	Bottom = 2,
	Hand = 3,
	Face = 4,
	FaceOrnament = 5,
	HeadOrnament = 6,
	BackOrnament = 7,
	EUGCAvatarType_MAX = 8
};

// Object: Enum Feature_UGC.EUGCAppearanceType
enum class EUGCAppearanceType : uint8_t
{
	None = 0,
	Avatar = 1,
	Binding = 2,
	All = 3,
	EUGCAppearanceType_MAX = 4
};

// Object: Enum Feature_UGC.ENPCCharacterBindPart
enum class ENPCCharacterBindPart : uint8_t
{
	Head_Left = 0,
	Head_Right = 1,
	Head = 2,
	Arm_Left = 3,
	Hand_Left = 4,
	Arm_Right = 5,
	Hand_Right = 6,
	Chest = 7,
	Waist = 8,
	Leg_Left = 9,
	Leg_Right = 10,
	Foot_Left = 11,
	Foot_Right = 12,
	Max = 13
};

// Object: Enum Feature_UGC.EUGCAnimTragetRoleType
enum class EUGCAnimTragetRoleType : uint8_t
{
	EUGCATRT_Character = 0,
	EUGCATRT_Quadruped = 1,
	EUGCATRT_MAX = 2
};

// Object: Enum Feature_UGC.EUGCAnimType
enum class EUGCAnimType : uint8_t
{
	EUGCAnimType_AnimSequence = 0,
	EUGCAnimType_AIGCPoseAnim = 1,
	EUGCAnimType_PoseAnim = 2,
	EUGCAnimType_UGCMontageAnim = 3,
	EUGCAnimType_MAX = 4
};

// Object: Enum Feature_UGC.EUGCRPCType
enum class EUGCRPCType : uint8_t
{
	LocalCall = 0,
	ToServer = 1,
	ToClient = 2,
	Broadcast = 3,
	EUGCRPCType_MAX = 4
};

// Object: Enum Feature_UGC.ECrowdV2UGCNPCState
enum class ECrowdV2UGCNPCState : uint8_t
{
	Idle = 0,
	Walk = 1,
	Run = 2,
	None = 3,
	ECrowdV2UGCNPCState_MAX = 4
};

// Object: Enum Feature_UGC.EUGCAbility
enum class EUGCAbility : uint8_t
{
	Default = 0,
	Appearance = 1,
	Functional = 2,
	EUGCAbility_MAX = 3
};

// Object: Enum Feature_UGC.EUGCCloneState
enum class EUGCCloneState : uint8_t
{
	Ready = 0,
	Running = 1,
	Canceled = 2,
	Completed = 3,
	EUGCCloneState_MAX = 4
};

// Object: Enum Feature_UGC.EUGCProcessState
enum class EUGCProcessState : uint8_t
{
	None = 0,
	Ready = 1,
	Pending = 2,
	Start = 3,
	Loading = 4,
	Finished = 5,
	EUGCProcessState_MAX = 6
};

// Object: Enum Feature_UGC.EBuffOverlapRule
enum class EBuffOverlapRule : uint8_t
{
	Refresh = 0,
	Maintain = 1,
	EBuffOverlapRule_MAX = 2
};

// Object: Enum Feature_UGC.EBuffEffect
enum class EBuffEffect : uint8_t
{
	BeginEffect = 0,
	PulseEffect = 1,
	EndEffect = 2,
	EBuffEffect_MAX = 3
};

// Object: Enum Feature_UGC.EImpulseType
enum class EImpulseType : uint8_t
{
	Direction = 0,
	Explode = 1,
	Radial = 2,
	EImpulseType_MAX = 3
};

// Object: Enum Feature_UGC.EEffectApplyOpportunity
enum class EEffectApplyOpportunity : uint8_t
{
	EAO_Timeline = 0,
	EAO_PrecastBegin = 1,
	EAO_PrecastEnd = 2,
	EAO_ChannelingUp = 3,
	EAO_ChannelingFull = 4,
	EAO_AttackBegin = 5,
	EAO_BackSwingBegin = 6,
	EAO_BackSwingEnd = 7,
	EAO_MAX = 8
};

// Object: Enum Feature_UGC.ESkillEvent
enum class ESkillEvent : uint8_t
{
	StartCast = 0,
	PrecastStageStart = 1,
	PrecastStageEnd = 2,
	ChannelingStageStart = 3,
	ChannelingStageEnd = 4,
	ChannelingStageChargeFull = 5,
	AttackStageStart = 6,
	AttackStageEnd = 7,
	BackSwingStageStart = 8,
	BackSwingStageEnd = 9,
	Interrupted = 10,
	EndCast = 11,
	ESkillEvent_MAX = 12
};

// Object: Enum Feature_UGC.ETargetRelation
enum class ETargetRelation : uint8_t
{
	Invalid = 0,
	etrSelf = 1,
	etrEmeny = 2,
	etrNeutral = 4,
	etrFriend = 8,
	ETargetRelation_MAX = 9
};

// Object: Enum Feature_UGC.ECampRelation
enum class ECampRelation : uint8_t
{
	Relation_Invalid = 0,
	Relation_Enemy = 1,
	Relation_Neutral = 2,
	Relation_Friendly = 3,
	Relation_MAX = 4
};

// Object: Enum Feature_UGC.ESkillAOEShape
enum class ESkillAOEShape : uint8_t
{
	Invalid = 0,
	Circle = 1,
	Sector = 2,
	Rectangle = 3,
	Self = 4,
	Weapon = 5,
	ESkillAOEShape_MAX = 6
};

// Object: Enum Feature_UGC.ESkillInfoTimePoint
enum class ESkillInfoTimePoint : uint8_t
{
	None = 0,
	Start = 1,
	AttackStage = 2,
	ComboNotContinued = 3,
	ESkillInfoTimePoint_MAX = 4
};

// Object: Enum Feature_UGC.ESkillTiggerEffect
enum class ESkillTiggerEffect : uint8_t
{
	Effect_Damage = 0,
	Effect_AddSelfBuff = 1,
	Effect_LaunchProjectile = 2,
	Effect_CallAI = 3,
	Effect_ScalingMesh = 4,
	Effect_MAX = 5
};

// Object: Enum Feature_UGC.ESkillTriggerOpportunity
enum class ESkillTriggerOpportunity : uint8_t
{
	Opportunity_Hit = 0,
	Opportunity_Launch = 1,
	Opportunity_SpecifyTime = 2,
	Opportunity_MAX = 3
};

// Object: Enum Feature_UGC.ESkillType
enum class ESkillType : uint8_t
{
	Type_General = 0,
	Type_ChargeSkill = 1,
	Type_NoneAnim = 2,
	Type_MAX = 3
};

// Object: Enum Feature_UGC.ESkillAllowUpperBodyAniStageMask
enum class ESkillAllowUpperBodyAniStageMask : uint8_t
{
	PrecastStage = 2,
	ChannelingStage = 4,
	AttackStage = 8,
	ESkillAllowUpperBodyAniStageMask_MAX = 9
};

// Object: Enum Feature_UGC.ESkillStageType
enum class ESkillStageType : uint8_t
{
	Stage_None = 0,
	Stage_PrecastStage = 1,
	Stage_ChannelingStage = 2,
	Stage_AttackStage = 3,
	Stage_BackSwingStage = 4,
	Stage_MAX = 5
};

// Object: Enum Feature_UGC.EActionScope
enum class EActionScope : uint8_t
{
	Scope_Begin = 0,
	Scope_End = 1,
	Scope_MAX = 2
};

// Object: Enum Feature_UGC.ESkillParticleAttachPoint
enum class ESkillParticleAttachPoint : uint8_t
{
	BottomCenter = 0,
	Center = 1,
	TopCenter = 2,
	ESkillParticleAttachPoint_MAX = 3
};

// Object: Enum Feature_UGC.ESkillProjectileShape
enum class ESkillProjectileShape : uint8_t
{
	Sphere = 0,
	Box = 1,
	Capsule = 2,
	ESkillProjectileShape_MAX = 3
};

// Object: Enum Feature_UGC.EUGCTimelineDirection
enum class EUGCTimelineDirection : uint8_t
{
	Forward = 0,
	Backward = 1,
	EUGCTimelineDirection_MAX = 2
};

// Object: Enum Feature_UGC.EUGCTimelineLengthMode
enum class EUGCTimelineLengthMode : uint8_t
{
	TL_TimelineLength = 0,
	TL_LastKeyFrame = 1,
	TL_MAX = 2
};

// Object: Enum Feature_UGC.EUGCGunWeaponTrajectoryType
enum class EUGCGunWeaponTrajectoryType : uint8_t
{
	Instant = 0,
	Projectile = 1,
	EUGCGunWeaponTrajectoryType_MAX = 2
};

// Object: Enum Feature_UGC.EUGCGunWeaponAddBuffWay
enum class EUGCGunWeaponAddBuffWay : uint8_t
{
	Directly = 0,
	Probability = 1,
	Count = 2,
	Cooldown = 3,
	EUGCGunWeaponAddBuffWay_MAX = 4
};

// Object: Enum Feature_UGC.EUGCGunWeaponAddBuffTarget
enum class EUGCGunWeaponAddBuffTarget : uint8_t
{
	Self = 0,
	Enemy = 1,
	EUGCGunWeaponAddBuffTarget_MAX = 2
};

// Object: Enum Feature_UGC.EUGCGunWeaponAddBuffCondition
enum class EUGCGunWeaponAddBuffCondition : uint8_t
{
	HitTarget = 0,
	StartFire = 1,
	EUGCGunWeaponAddBuffCondition_MAX = 2
};

// Object: Enum Feature_UGC.EUGCGunWeaponAimType
enum class EUGCGunWeaponAimType : uint8_t
{
	None = 0,
	Custom = 1,
	IronSights = 2,
	EUGCGunWeaponAimType_MAX = 3
};

// Object: Enum Feature_UGC.EUGCAssistAimSolution
enum class EUGCAssistAimSolution : uint8_t
{
	None = 0,
	Default = 1,
	Thin = 2,
	Moderate = 3,
	Thick = 4,
	Custom = 5,
	EUGCAssistAimSolution_MAX = 6
};

// Object: Enum Feature_UGC.EUGCWeaponFire
enum class EUGCWeaponFire : uint8_t
{
	Once = 1,
	Pulse = 2,
	EUGCWeaponFire_MAX = 3
};

// Object: Enum Feature_UGC.EUGCWeaponLevel
enum class EUGCWeaponLevel : uint8_t
{
	TinyThin = 1,
	Thin = 2,
	Moderate = 3,
	Thick = 4,
	UtmostThick = 5,
	EUGCWeaponLevel_MAX = 6
};

// Object: Enum Feature_UGC.EUGCWeaponTable
enum class EUGCWeaponTable : uint8_t
{
	AssistAimDamp = 0,
	AssistAimAttract = 1,
	AssistAimLock = 2,
	EUGCWeaponTable_MAX = 3
};

// Object: Enum Feature_UGC.EEffectType
enum class EEffectType : uint8_t
{
	Muzzle = 0,
	BulletLine = 1,
	EjectionCase = 2,
	Hit = 3,
	EEffectType_MAX = 4
};

// Object: Enum Feature_UGC.EUGCBodyPart
enum class EUGCBodyPart : uint8_t
{
	Unkown = 0,
	Head = 1,
	LeftArm = 2,
	RightArm = 3,
	LeftHand = 4,
	RightHand = 5,
	Chest = 6,
	Waist = 7,
	LeftLeg = 8,
	RightLeg = 9,
	LeftFoot = 10,
	RightFoot = 11,
	EUGCBodyPart_MAX = 12
};

// Object: Enum Feature_UGC.EUGCWeaponState
enum class EUGCWeaponState : uint8_t
{
	EWS_None = 0,
	EWS_Active = 1,
	EWS_Inactive = 2,
	EWS_Equipping = 3,
	EWS_Unequipping = 4,
	EWS_Ready = 5,
	EWS_Firing = 6,
	EWS_ClipChanging = 7,
	EWS_PreAiming = 8,
	EWS_AimFiring = 9,
	EWS_PostAiming = 10,
	EWS_MAX = 11
};

// Object: Enum Feature_UGC.ESkinMeshType
enum class ESkinMeshType : uint8_t
{
	StaticMesh = 0,
	SkeletalMesh = 1,
	ESkinMeshType_MAX = 2
};

// Object: Enum Feature_UGC.EResetSkinType
enum class EResetSkinType : uint8_t
{
	ResetMesh = 0,
	ResetMaterial = 1,
	EResetSkinType_MAX = 2
};

// Object: Enum Feature_UGC.EUSIndicatorType
enum class EUSIndicatorType : uint8_t
{
	SelfCircle = 1,
	LineDirection = 2,
	TargetCirclePos = 3,
	LineAddCircleCircleDirection = 4,
	Sector = 5,
	RangeTarget = 6,
	EUSIndicatorType_MAX = 7
};

// Package: RuntimeGeometryUtils
// Enums: 11

// Object: Enum RuntimeGeometryUtils.EDynamicMeshActorCollisionMode
enum class EDynamicMeshActorCollisionMode : uint8_t
{
	NoCollision = 0,
	ComplexAsSimple = 1,
	ComplexAsSimpleAsync = 2,
	SimpleConvexHull = 3,
	EDynamicMeshActorCollisionMode_MAX = 4
};

// Object: Enum RuntimeGeometryUtils.EDynamicMeshActorBooleanOperation
enum class EDynamicMeshActorBooleanOperation : uint8_t
{
	Union = 0,
	Subtraction = 1,
	Intersection = 2,
	EDynamicMeshActorBooleanOperation_MAX = 3
};

// Object: Enum RuntimeGeometryUtils.EDynamicMeshActorPrimitiveType
enum class EDynamicMeshActorPrimitiveType : uint8_t
{
	Sphere = 0,
	Box = 1,
	EDynamicMeshActorPrimitiveType_MAX = 2
};

// Object: Enum RuntimeGeometryUtils.EDynamicMeshActorSourceType
enum class EDynamicMeshActorSourceType : uint8_t
{
	Primitive = 0,
	ImportedMesh = 1,
	ExternallyGenerated = 2,
	EDynamicMeshActorSourceType_MAX = 3
};

// Object: Enum RuntimeGeometryUtils.EDynamicMeshActorNormalsMode
enum class EDynamicMeshActorNormalsMode : uint8_t
{
	SplitNormals = 0,
	PerVertexNormals = 1,
	FaceNormals = 2,
	EDynamicMeshActorNormalsMode_MAX = 3
};

// Object: Enum RuntimeGeometryUtils.EGeneratedMeshBooleanOperation
enum class EGeneratedMeshBooleanOperation : uint8_t
{
	Union = 0,
	Subtraction = 1,
	Intersection = 2,
	EGeneratedMeshBooleanOperation_MAX = 3
};

// Object: Enum RuntimeGeometryUtils.ERuntimeStaticMeshCollisionType
enum class ERuntimeStaticMeshCollisionType : uint8_t
{
	None = 0,
	Simple = 1,
	Complex = 2,
	ERuntimeStaticMeshCollisionType_MAX = 3
};

// Object: Enum RuntimeGeometryUtils.EMoeDynamicMeshBooleanOperation
enum class EMoeDynamicMeshBooleanOperation : uint8_t
{
	None = 0,
	Subtraction = 1,
	Intersection = 2,
	Union = 3,
	EMoeDynamicMeshBooleanOperation_MAX = 4
};

// Object: Enum RuntimeGeometryUtils.EMoeDynamicMeshCollisionMode
enum class EMoeDynamicMeshCollisionMode : uint8_t
{
	NoCollision = 0,
	SimpleAsComplex = 1,
	ComplexAsSimple = 2,
	ComplexAsSimpleAsync = 3,
	SimpleConvexHull = 4,
	EMoeDynamicMeshCollisionMode_MAX = 5
};

// Object: Enum RuntimeGeometryUtils.EMoeDynamicMeshNormalsMode
enum class EMoeDynamicMeshNormalsMode : uint8_t
{
	SplitNormals = 0,
	PerVertexNormals = 1,
	FaceNormals = 2,
	EMoeDynamicMeshNormalsMode_MAX = 3
};

// Object: Enum RuntimeGeometryUtils.EMoeDynamicMeshType
enum class EMoeDynamicMeshType : uint8_t
{
	Cone = 0,
	Cube = 1,
	Slop = 2,
	FrustumCircle = 3,
	Cylinder = 4,
	Torus = 5,
	Cutter = 6,
	Cutter2 = 7,
	Composite = 8,
	EMoeDynamicMeshType_MAX = 9
};

// Package: TBUMCPBridge
// Enums: 1

// Object: Enum TBUMCPBridge.ECSVibeConnectionState
enum class ECSVibeConnectionState : uint8_t
{
	Disconnected = 0,
	Connecting = 1,
	Connected = 2,
	Error = 3,
	ECSVibeConnectionState_MAX = 4
};

// Package: AIGCKit
// Enums: 2

// Object: Enum AIGCKit.EAIGCAnimVersion
enum class EAIGCAnimVersion : uint8_t
{
	EAIGCAnimVersion_InValid = 0,
	EAIGCAnimVersion_UnCompress = 1,
	EAIGCAnimVersion_Compress = 2,
	EAIGCAnimVersion_MAX = 3
};

// Object: Enum AIGCKit.EBoneControlMode
enum class EBoneControlMode : uint8_t
{
	BCM_Ignore = 0,
	BCM_HEAD = 1,
	BCM_BODY = 2,
	BCM_MAX = 3
};

// Package: AIGCTTS
// Enums: 1

// Object: Enum AIGCTTS.EDataHandlerType
enum class EDataHandlerType : uint8_t
{
	LLMText = 0,
	Song = 1,
	Greet = 2,
	Security = 3,
	EDataHandlerType_MAX = 4
};

// Package: SocketIOClient
// Enums: 2

// Object: Enum SocketIOClient.ESIOConnectionCloseReason
enum class ESIOConnectionCloseReason : uint8_t
{
	CLOSE_REASON_NORMAL = 0,
	CLOSE_REASON_DROP = 1,
	CLOSE_REASON_MAX = 2
};

// Object: Enum SocketIOClient.ESIOThreadOverrideOption
enum class ESIOThreadOverrideOption : uint8_t
{
	USE_DEFAULT = 0,
	USE_GAME_THREAD = 1,
	USE_NETWORK_THREAD = 2,
	USE_MAX = 3
};

// Package: SIOJson
// Enums: 4

// Object: Enum SIOJson.ESIOJson
enum class ESIOJson : uint8_t
{
	None = 0,
	Null = 1,
	String = 2,
	Number = 3,
	Boolean = 4,
	Array = 5,
	Object = 6,
	Binary = 7,
	ESIOJson_MAX = 8
};

// Object: Enum SIOJson.ESIORequestStatus
enum class ESIORequestStatus : uint8_t
{
	NotStarted = 0,
	Processing = 1,
	Failed = 2,
	Failed_ConnectionError = 3,
	Succeeded = 4,
	ESIORequestStatus_MAX = 5
};

// Object: Enum SIOJson.ESIORequestContentType
enum class ESIORequestContentType : uint8_t
{
	x_www_form_urlencoded_url = 0,
	x_www_form_urlencoded_body = 1,
	json = 2,
	binary = 3,
	ESIORequestContentType_MAX = 4
};

// Object: Enum SIOJson.ESIORequestVerb
enum class ESIORequestVerb : uint8_t
{
	GET = 0,
	POST = 1,
	PUT = 2,
	DEL = 3,
	CUSTOM = 4,
	ESIORequestVerb_MAX = 5
};

// Package: CoreUtility
// Enums: 2

// Object: Enum CoreUtility.ESIOCallbackType
enum class ESIOCallbackType : uint8_t
{
	CALLBACK_GAME_THREAD = 0,
	CALLBACK_BACKGROUND_THREADPOOL = 1,
	CALLBACK_BACKGROUND_TASKGRAPH = 2,
	CALLBACK_MAX = 3
};

// Object: Enum CoreUtility.EImageFormatBPType
enum class EImageFormatBPType : uint8_t
{
	Invalid = 0,
	PNG = 1,
	JPEG = 2,
	GrayscaleJPEG = 3,
	BMP = 4,
	ICO = 5,
	EXR = 6,
	ICNS = 7,
	EImageFormatBPType_MAX = 8
};

// Package: MoePerformanceEstimationRuntime
// Enums: 6

// Object: Enum MoePerformanceEstimationRuntime.ERegionHotSpot
enum class ERegionHotSpot : uint8_t
{
	RHS_TranslucencyOverdraw = 0,
	RHS_MovableObjects = 1,
	RHS_Objects = 2,
	RHS_OverlapComponents = 3,
	RHS_Num = 4,
	RHS_MAX = 5
};

// Object: Enum MoePerformanceEstimationRuntime.EGlobalHotSpot
enum class EGlobalHotSpot : uint8_t
{
	GHS_MovableObjects = 0,
	GHS_OverlapComponents = 1,
	GHS_WholeScenePrimitives = 2,
	GHS_TickCost = 3,
	GHS_EditableMesh = 4,
	GHS_MemoryObject = 5,
	GHS_TickCostSingleObject = 6,
	GHS_FakeLight = 7,
	GHS_Num = 8,
	GHS_MAX = 9
};

// Object: Enum MoePerformanceEstimationRuntime.EPerformanceRequirement
enum class EPerformanceRequirement : uint8_t
{
	VeryHigh = 0,
	High = 1,
	Medium = 2,
	Low = 3,
	VeryLow = 4,
	ExtremeHigh = 5,
	EPerformanceRequirement_MAX = 6
};

// Object: Enum MoePerformanceEstimationRuntime.EDeviceLevel
enum class EDeviceLevel : uint8_t
{
	VeryHigh = 0,
	High = 1,
	Medium = 2,
	Low = 3,
	VeryLow = 4,
	EDeviceLevel_MAX = 5
};

// Object: Enum MoePerformanceEstimationRuntime.EHighlightIndex
enum class EHighlightIndex : uint8_t
{
	EHI_Low = 0,
	EHI_Middle = 1,
	EHI_High = 2,
	ERC_NUM = 3,
	EHighlightIndex_MAX = 4
};

// Object: Enum MoePerformanceEstimationRuntime.ERiskCategories
enum class ERiskCategories : uint8_t
{
	ERC_Moveable = 0,
	ERC_Overlap = 1,
	ERC_EditableMesh = 2,
	ERC_MemoryObject = 3,
	ERC_TickCost = 4,
	ERC_FakeLight = 5,
	ERC_NUM = 6,
	ERC_MAX = 7
};

// Package: SceneProfiler
// Enums: 6

// Object: Enum SceneProfiler.ELookDevAnimType
enum class ELookDevAnimType : uint8_t
{
	Idle = 0,
	IdleShow = 1,
	Sprinted = 2,
	Dive = 3,
	Jump = 4,
	Rebirth = 5,
	Move = 6,
	Enter = 7,
	Surrounding = 8,
	Handhold = 9,
	Action1P = 10,
	Action2P = 11,
	None = 12,
	ELookDevAnimType_MAX = 13
};

// Object: Enum SceneProfiler.EAnimationState
enum class EAnimationState : uint8_t
{
	AnimBlueprint = 0,
	AnimSequence = 1,
	Num = 2,
	EAnimationState_MAX = 3
};

// Object: Enum SceneProfiler.ELookDevSamplingSourceType
enum class ELookDevSamplingSourceType : uint8_t
{
	Avatar = 0,
	Particle = 1,
	ELookDevSamplingSourceType_MAX = 2
};

// Object: Enum SceneProfiler.EDisplayActorType
enum class EDisplayActorType : uint8_t
{
	DisplayActorType_Bag = 0,
	DisplayActorType_Lobby_LOD0 = 1,
	DisplayActorType_Lobby_LOD1 = 2,
	Num = 3,
	EDisplayActorType_MAX = 4
};

// Object: Enum SceneProfiler.ELookDevItemType
enum class ELookDevItemType : uint8_t
{
	BackpackItem_Suit = 0,
	BackpackItem_HandOrnament = 1,
	BackpackItem_HandOrnamentSummon = 2,
	BackpackItem_HeadWear = 3,
	BackpackItem_FaceOrnament = 4,
	BackpackItem_BackOrnament = 5,
	BackpackItem_Surrounding = 6,
	BackpackItem_Vehicle = 7,
	BackpackItem_Action1P = 8,
	BackpackItem_Action2P = 9,
	Num = 10,
	ELookDevItemType_MAX = 11
};

// Object: Enum SceneProfiler.EEmitterLevel
enum class EEmitterLevel : uint8_t
{
	EL_ALL = 0,
	EL_High = 1,
	EL_Mid = 2,
	EL_Low = 3,
	End = 4,
	EEmitterLevel_MAX = 5
};

// Package: GVoiceSDK
// Enums: 12

// Object: Enum GVoiceSDK.EGVoiceChatAttenuationModel
enum class EGVoiceChatAttenuationModel : uint8_t
{
	None = 0,
	InverseByDistance = 1,
	ExponentialByDistance = 2,
	EGVoiceChatAttenuationModel_MAX = 3
};

// Object: Enum GVoiceSDK.EGVoiceSoundEffects
enum class EGVoiceSoundEffects : uint8_t
{
	SoundEffectsRevbChurch = 0,
	SoundEffectsRevbTheater = 1,
	SoundEffectsHell = 2,
	SoundEffectsRobot1 = 3,
	SoundEffectsMaleToFemale = 4,
	SoundEffectsFemaleToMale = 5,
	SoundEffectsDrunk = 6,
	SoundEffectsPapiJiang = 7,
	SoundEffectsSquirrel = 8,
	SoundEffectsNoEffect = 9,
	EGVoiceSoundEffects_MAX = 10
};

// Object: Enum GVoiceSDK.EGVoiceMemberRole
enum class EGVoiceMemberRole : uint8_t
{
	MemberRoleInvalid = 0,
	MemberRoleAnchor = 1,
	MemberRoleAudience = 2,
	EGVoiceMemberRole_MAX = 3
};

// Object: Enum GVoiceSDK.EGVoiceVoiceMode
enum class EGVoiceVoiceMode : uint8_t
{
	ModeRealTime = 0,
	ModeMessages = 1,
	ModeTranslation = 2,
	ModeRSTT = 3,
	ModeHighQuality = 4,
	ModeRSTS = 5,
	EGVoiceVoiceMode_MAX = 6
};

// Object: Enum GVoiceSDK.EGVoiceDeviceType
enum class EGVoiceDeviceType : uint8_t
{
	RENDER_DEVICE = 0,
	CAPTURE_DEVICE = 1,
	EGVoiceDeviceType_MAX = 2
};

// Object: Enum GVoiceSDK.EGVoiceDeviceState
enum class EGVoiceDeviceState : uint8_t
{
	DeviceStateUnconnected = 0,
	DeviceStateWriteHeadsetConnected = 1,
	DeviceStateBluetoothConnected = 2,
	EGVoiceDeviceState_MAX = 3
};

// Object: Enum GVoiceSDK.EGVoiceEvent
enum class EGVoiceEvent : uint8_t
{
	EventNoDeviceConnected = 0,
	EventHeadsetDisconnected = 10,
	EventHeadsetConnected = 11,
	EventBluetoothHeadsetDisconnected = 20,
	EventBluetoothHeadsetConnected = 21,
	EventMicStateOpenSucc = 30,
	EventMicStateOpenErr = 31,
	EventMicStateNoOpen = 32,
	EventMicStateOccupancy = 33,
	EventSpeakerStateOpenSucc = 40,
	EventSpeakerStateOpenErr = 41,
	EventSpeakerStateNoOpen = 42,
	EventAudioInterruptBegin = 50,
	EventAudioInterruptEnd = 51,
	EventAudioRecoderException = 52,
	EventAudioRenderException = 53,
	EventPhoneCallPickUp = 54,
	EventPhoneCallHangUp = 55,
	EGVoiceEvent_MAX = 56
};

// Object: Enum GVoiceSDK.EGVoiceSoundTag
enum class EGVoiceSoundTag : uint8_t
{
	GVST_kNone = 0,
	GVST_kLow = 1,
	GVST_kBright = 2,
	GVST_kMagnetic = 3,
	GVST_kSoft = 4,
	GVST_kComposed = 5,
	GVST_kSweet = 6,
	GVST_kRecord = 7,
	GVST_kLolita = 8,
	GVST_kUncle = 9,
	GVST_kMinions = 10,
	GVST_kSweetLolita = 11,
	GVST_kOldman = 12,
	GVST_kAlien = 13,
	GVST_kRobot = 14,
	GVST_kGiantRobot = 15,
	GVST_kAstronaut = 16,
	GVST_kVocalE = 17,
	GVST_kChurch = 18,
	GVST_kGlass = 19,
	GVST_kHall = 20,
	GVST_kStudy = 21,
	GVST_kTheater = 22,
	GVST_kLecture = 23,
	GVST_kStereo = 24,
	GVST_kDynamic3D = 25,
	GVST_kFlanger = 26,
	GVST_kVibrato = 27,
	GVST_kChrous = 28,
	GVST_kChangeTempo = 29,
	GVST_kChangePitch = 30,
	GVST_kChangeRate = 31,
	GVST_kUserEQ = 32,
	GVST_kBoy = 33,
	GVST_kGirl = 34,
	GVST_MAX = 35
};

// Object: Enum GVoiceSDK.EGVoiceUrlType
enum class EGVoiceUrlType : uint8_t
{
	GV_URL_INVALID = 0,
	GV_URL_TRANSLATE = 1,
	GV_URL_REMOTE_CONFIG = 2,
	GV_URL_LOG_REPORT = 3,
	GV_URL_VOICE_REPORT = 4,
	GV_URL_FLQ = 5,
	GV_URL_DATA_REPORT = 6,
	GV_URL_MAX = 7
};

// Object: Enum GVoiceSDK.EGVoiceRealTimeTranslateType
enum class EGVoiceRealTimeTranslateType : uint8_t
{
	RT_TRANSLATE_INVALID = 0,
	RT_TRANSLATE_TEXT = 1,
	RT_TRANSLATE_SPEECH = 2,
	RT_TRANSLATE_SPEECH_AND_TEXT = 3,
	RT_TRANSLATE_MAX = 4
};

// Object: Enum GVoiceSDK.EGVoiceSpeechTranslateType
enum class EGVoiceSpeechTranslateType : uint8_t
{
	SPEECH_TRANSLATE_STST = 0,
	SPEECH_TRANSLATE_STTT = 1,
	SPEECH_TRANSLATE_STTS = 2,
	SPEECH_TRANSLATE_MAX = 3
};

// Object: Enum GVoiceSDK.EGVoiceSpeechLanguageType
enum class EGVoiceSpeechLanguageType : uint8_t
{
	SPEECH_LANGUAGE_ZH = 0,
	SPEECH_LANGUAGE_EN = 1,
	SPEECH_LANGUAGE_JA = 2,
	SPEECH_LANGUAGE_KO = 3,
	SPEECH_LANGUAGE_DE = 4,
	SPEECH_LANGUAGE_FR = 5,
	SPEECH_LANGUAGE_ES = 6,
	SPEECH_LANGUAGE_IT = 7,
	SPEECH_LANGUAGE_TR = 8,
	SPEECH_LANGUAGE_RU = 9,
	SPEECH_LANGUAGE_PT = 10,
	SPEECH_LANGUAGE_VI = 11,
	SPEECH_LANGUAGE_ID = 12,
	SPEECH_LANGUAGE_MS = 13,
	SPEECH_LANGUAGE_TH = 14,
	SPEECH_LANGUAGE_ZH_TW = 15,
	SPEECH_LANGUAGE_AF = 16,
	SPEECH_LANGUAGE_SQ = 17,
	SPEECH_LANGUAGE_AM = 18,
	SPEECH_LANGUAGE_AR = 19,
	SPEECH_LANGUAGE_HY = 20,
	SPEECH_LANGUAGE_AZ = 21,
	SPEECH_LANGUAGE_EU = 22,
	SPEECH_LANGUAGE_BN = 23,
	SPEECH_LANGUAGE_BS = 24,
	SPEECH_LANGUAGE_BG = 25,
	SPEECH_LANGUAGE_MY = 26,
	SPEECH_LANGUAGE_CA = 27,
	SPEECH_LANGUAGE_HR = 28,
	SPEECH_LANGUAGE_CS = 29,
	SPEECH_LANGUAGE_DA = 30,
	SPEECH_LANGUAGE_NL = 31,
	SPEECH_LANGUAGE_ET = 32,
	SPEECH_LANGUAGE_FIL = 33,
	SPEECH_LANGUAGE_FI = 34,
	SPEECH_LANGUAGE_GL = 35,
	SPEECH_LANGUAGE_KA = 36,
	SPEECH_LANGUAGE_EL = 37,
	SPEECH_LANGUAGE_GU = 38,
	SPEECH_LANGUAGE_IW = 39,
	SPEECH_LANGUAGE_HI = 40,
	SPEECH_LANGUAGE_HU = 41,
	SPEECH_LANGUAGE_IS = 42,
	SPEECH_LANGUAGE_JV = 43,
	SPEECH_LANGUAGE_KN = 44,
	SPEECH_LANGUAGE_KK = 45,
	SPEECH_LANGUAGE_KM = 46,
	SPEECH_LANGUAGE_LO = 47,
	SPEECH_LANGUAGE_LV = 48,
	SPEECH_LANGUAGE_LT = 49,
	SPEECH_LANGUAGE_MK = 50,
	SPEECH_LANGUAGE_ML = 51,
	SPEECH_LANGUAGE_MR = 52,
	SPEECH_LANGUAGE_MN = 53,
	SPEECH_LANGUAGE_NE = 54,
	SPEECH_LANGUAGE_NO = 55,
	SPEECH_LANGUAGE_FA = 56,
	SPEECH_LANGUAGE_PL = 57,
	SPEECH_LANGUAGE_PA = 58,
	SPEECH_LANGUAGE_RO = 59,
	SPEECH_LANGUAGE_SR = 60,
	SPEECH_LANGUAGE_SI = 61,
	SPEECH_LANGUAGE_SK = 62,
	SPEECH_LANGUAGE_SL = 63,
	SPEECH_LANGUAGE_SU = 64,
	SPEECH_LANGUAGE_SW = 65,
	SPEECH_LANGUAGE_SV = 66,
	SPEECH_LANGUAGE_TA = 67,
	SPEECH_LANGUAGE_TE = 68,
	SPEECH_LANGUAGE_UK = 69,
	SPEECH_LANGUAGE_UR = 70,
	SPEECH_LANGUAGE_UZ = 71,
	SPEECH_LANGUAGE_ZU = 72,
	SPEECH_LANGUAGE_CNT = 73,
	SPEECH_LANGUAGE_MAX = 74
};

// Package: MoePreDownload
// Enums: 1

// Object: Enum MoePreDownload.EMoePreDownloadFileTaskState
enum class EMoePreDownloadFileTaskState : uint8_t
{
	None = 0,
	Pending = 1,
	Success = 2,
	Failed = 3,
	EMoePreDownloadFileTaskState_MAX = 4
};

// Package: AdvancedThirdPersonCamera
// Enums: 3

// Object: Enum AdvancedThirdPersonCamera.EATPCInterpolationType
enum class EATPCInterpolationType : uint8_t
{
	SmoothInterpolation = 0,
	ConstInterpolation = 1,
	EATPCInterpolationType_MAX = 2
};

// Object: Enum AdvancedThirdPersonCamera.EATPCChangeTargetReason
enum class EATPCChangeTargetReason : uint8_t
{
	SetNew = 0,
	LostByExitCameraMode = 1,
	LostByDistance = 2,
	LostByVisible = 3,
	LostByPlayerInput = 4,
	EATPCChangeTargetReason_MAX = 5
};

// Object: Enum AdvancedThirdPersonCamera.EATPCCameraModeScriptTickGroup
enum class EATPCCameraModeScriptTickGroup : uint8_t
{
	PreCameraObjectTick = 0,
	PostCameraObjectTick = 1,
	EATPCCameraModeScriptTickGroup_MAX = 2
};

// Package: PrefabricatorRuntime
// Enums: 1

// Object: Enum PrefabricatorRuntime.EPrefabricatorPivotPosition
enum class EPrefabricatorPivotPosition : uint8_t
{
	ExtremeLeft = 0,
	ExtremeRight = 1,
	Center = 2,
	EPrefabricatorPivotPosition_MAX = 3
};

// Package: ConstructionSystemRuntime
// Enums: 3

// Object: Enum ConstructionSystemRuntime.EConstructionSystemToolType
enum class EConstructionSystemToolType : uint8_t
{
	BuildTool = 0,
	RemoveTool = 1,
	EConstructionSystemToolType_MAX = 2
};

// Object: Enum ConstructionSystemRuntime.EConstructionSystemCursorVisiblity
enum class EConstructionSystemCursorVisiblity : uint8_t
{
	Visible = 0,
	VisibleInvalid = 1,
	Hidden = 2,
	EConstructionSystemCursorVisiblity_MAX = 3
};

// Object: Enum ConstructionSystemRuntime.EPrefabricatorConstructionSnapType
enum class EPrefabricatorConstructionSnapType : uint8_t
{
	Floor = 0,
	Wall = 1,
	Object = 2,
	EPrefabricatorConstructionSnapType_MAX = 3
};

// Package: Feature_Arena
// Enums: 186

// Object: Enum Feature_Arena.EArenaTimelineCancelReason
enum class EArenaTimelineCancelReason : uint8_t
{
	None = 0,
	ServerCastSkillFailed = 1,
	FromSkillAttack = 2,
	MoveFromDelta = 3,
	FromIsDone = 4,
	FromInternalStart = 5,
	ComponentEndPlay = 6,
	FromBuffEnd = 7,
	FromTargetSelectFailure = 8,
	JinShenTask = 9,
	ClientBulletDestroy = 10,
	ArenaCarStop = 11,
	SubGameHasEnded = 12,
	InterruptTask = 13,
	ArenaCarPullEnemyOnCar = 14,
	YaoDetach = 15,
	CancelIndicatorPreparingSkill = 16,
	ServerBulletLogicDestroy = 17,
	SubGameHasEnded_EndPlay = 18,
	SubGameHasEnded_SubGameDestroy = 19,
	OnCharacterDead = 20,
	FromInternalBranch = 21,
	BounceBulletContinuousSelectTargetEnd = 22,
	JinShenSkill = 23,
	AblInterruptActionSkill = 24,
	HaiyueWorldChange = 25,
	PreFinalBattleStart = 26,
	FakeDeadInterrupt = 27,
	IllusionRemoved = 28,
	LaunchDeviceInterrupt = 29,
	BuffStateChange = 30,
	ManualUnEquip = 31,
	RenZheAttach = 32,
	SuperInterruptSkillMove = 33,
	AbnoramlWeakStun = 34,
	EArenaTimelineCancelReason_MAX = 35
};

// Object: Enum Feature_Arena.EArenaTimelineEndReason
enum class EArenaTimelineEndReason : uint8_t
{
	Successful = 0,
	Branched = 1,
	Interrupted = 2,
	EArenaTimelineEndReason_MAX = 3
};

// Object: Enum Feature_Arena.EArenaPathFollowingResult
enum class EArenaPathFollowingResult : uint8_t
{
	Success = 0,
	Blocked = 1,
	OffPath = 2,
	Aborted = 3,
	Invalid = 4,
	EArenaPathFollowingResult_MAX = 5
};

// Object: Enum Feature_Arena.ECastSubSkillInRangeCenterType
enum class ECastSubSkillInRangeCenterType : uint8_t
{
	CasterLocation = 0,
	TargetLocation = 1,
	HaiYueLevelPos = 2,
	ECastSubSkillInRangeCenterType_MAX = 3
};

// Object: Enum Feature_Arena.ETaskEndCondition
enum class ETaskEndCondition : uint8_t
{
	SkillEnd = 0,
	HitHero = 1,
	HitHeroAndEnv = 2,
	ETaskEndCondition_MAX = 3
};

// Object: Enum Feature_Arena.EArenaDirectHitTarget
enum class EArenaDirectHitTarget : uint8_t
{
	Self = 0,
	Instigator = 1,
	EArenaDirectHitTarget_MAX = 2
};

// Object: Enum Feature_Arena.EArenaFindLevelType
enum class EArenaFindLevelType : uint8_t
{
	CasterUID = 0,
	TargetUID = 1,
	EArenaFindLevelType_MAX = 2
};

// Object: Enum Feature_Arena.EInterruptSkillTaskType
enum class EInterruptSkillTaskType : uint8_t
{
	AIS_NotInUse = 0,
	AIS_TargetIsDead = 1,
	AIS_MAX = 2
};

// Object: Enum Feature_Arena.EPlayCameraAnimationType
enum class EPlayCameraAnimationType : uint8_t
{
	MeshDirection = 0,
	BulletDirection = 1,
	Type3 = 2,
	Type4 = 3,
	EPlayCameraAnimationType_MAX = 4
};

// Object: Enum Feature_Arena.EArenaTrailAttackMarkOffsetType
enum class EArenaTrailAttackMarkOffsetType : uint8_t
{
	None = 0,
	FaceDir = 1,
	IndicatorDir = 2,
	EArenaTrailAttackMarkOffsetType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaTrailAttackMarkCenterType
enum class EArenaTrailAttackMarkCenterType : uint8_t
{
	SkillCaster = 0,
	SkillTarget = 1,
	EArenaTrailAttackMarkCenterType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaSummonActionType
enum class EArenaSummonActionType : uint8_t
{
	None = 0,
	Attack = 1,
	MoveTo = 2,
	StopMove = 3,
	CastSkill = 4,
	SetAIStrategy = 5,
	MoveToOwner = 6,
	SetRespawning = 7,
	EArenaSummonActionType_MAX = 8
};

// Object: Enum Feature_Arena.ESubGameTypeGroup
enum class ESubGameTypeGroup : uint8_t
{
	ESGTG_Normal = 0,
	ESGTG_DrawCard = 1,
	ESGTG_HOKHeist = 2,
	ESGTG_Prepare = 3,
	ESGTG_MAX = 4
};

// Object: Enum Feature_Arena.EArenaSummonAIStateCheckMode
enum class EArenaSummonAIStateCheckMode : uint8_t
{
	HasAny = 0,
	HasNone = 1,
	EArenaSummonAIStateCheckMode_MAX = 2
};

// Object: Enum Feature_Arena.EArenaBBBoolCheckMode
enum class EArenaBBBoolCheckMode : uint8_t
{
	AnyTrue = 0,
	AllTrue = 1,
	EArenaBBBoolCheckMode_MAX = 2
};

// Object: Enum Feature_Arena.EArenaSummonAIStateOperation
enum class EArenaSummonAIStateOperation : uint8_t
{
	Set = 0,
	Clear = 1,
	EArenaSummonAIStateOperation_MAX = 2
};

// Object: Enum Feature_Arena.EArenaDealEffectByDamageTypeCommon
enum class EArenaDealEffectByDamageTypeCommon : uint8_t
{
	ARENA_DEDT_Damage = 0,
	ARENA_DEDT_ModAttr = 1,
	ARENA_DEDT_Heal = 2,
	ARENA_DEDT_MAX = 3
};

// Object: Enum Feature_Arena.EArenaBuffSkillHitConditionCommon
enum class EArenaBuffSkillHitConditionCommon : uint8_t
{
	Arena_SHC_None = 0,
	Arena_SHC_DefenderMoreThanCount = 1,
	Arena_SHC_DefenderLessThanCount = 2,
	Arena_SHC_DefenderHpLessThanPercentage = 3,
	Arena_SHC_DefenderHpMoreThanPercentage = 4,
	Arena_SHC_DefenderAnyoneIsInBrush = 5,
	Arena_SHC_DfdHaveBuffGroupId = 6,
	Arena_SHC_AtkHaveBuffGroupId = 7,
	Arena_SHC_CheckTargetFilter = 8,
	Arena_SHC_CheckTargetFilterDfd = 9,
	Arena_SHC_CheckFailTargetFilterDfd = 10,
	Arena_SHC_TargetHasTaggedBuff = 11,
	Arena_SHC_AtkActorType = 12,
	Arena_SHC_DfdActorType = 13,
	Arena_SHC_DfdSideId = 14,
	Arena_SHC_DfdIsBuffSource = 15,
	Arena_SHC_EffectConfigMatch = 16,
	Arena_SHC_DfdHasShield = 17,
	Arena_SHC_DfdNoShield = 18,
	Arena_SHC_DfdNoBuffGroupId = 19,
	Arena_SHC_AtkNotSkillIds = 20,
	Arena_SHC_SkillIsType = 21,
	Arena_SHC_MaxHpLess = 22,
	Arena_SHC_MaxHpMore = 23,
	Arena_SHC_DisMore = 24,
	Arena_SHC_DisLess = 25,
	Arena_SHC_InNormalMove = 26,
	Arena_SHC_NotInNormalMove = 27,
	Arena_SHC_AtkNoBuffGroupId = 28,
	Arena_SHC_AtkIsSameSideWithBuffSource = 29,
	Arena_SHC_MAX = 30
};

// Object: Enum Feature_Arena.EBuffReviveType
enum class EBuffReviveType : uint8_t
{
	NotSpecified = 0,
	HpPercentage = 1,
	HpUpToLiteral = 2,
	ReviveByAnotherRequest = 3,
	EBuffReviveType_MAX = 4
};

// Object: Enum Feature_Arena.EBuffLifeState
enum class EBuffLifeState : uint8_t
{
	Init = 0,
	Activated = 1,
	Deactivate = 2,
	Destroyed = 3,
	EBuffLifeState_MAX = 4
};

// Object: Enum Feature_Arena.EArenaBuffRepOpType
enum class EArenaBuffRepOpType : uint8_t
{
	Add = 0,
	Update = 1,
	Remove = 2,
	EArenaBuffRepOpType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaAttrCheckType
enum class EArenaAttrCheckType : uint8_t
{
	HPPercent = 0,
	EArenaAttrCheckType_MAX = 1
};

// Object: Enum Feature_Arena.EArenaSpawnBulletLocationSpace
enum class EArenaSpawnBulletLocationSpace : uint8_t
{
	World = 0,
	Local = 1,
	Target = 2,
	RegionCenter = 3,
	LocalOnSkillStart = 4,
	EArenaSpawnBulletLocationSpace_MAX = 5
};

// Object: Enum Feature_Arena.EBulletSpawnTargetEnum
enum class EBulletSpawnTargetEnum : uint8_t
{
	None = 0,
	Self = 1,
	Target = 2,
	Indicator = 3,
	FaceToIndicatorPos = 4,
	OneOfMultiIndicatorDir = 5,
	ValidPosForDirIndicator = 6,
	EBulletSpawnTargetEnum_MAX = 7
};

// Object: Enum Feature_Arena.EBulletFixedParabolaEndParamType
enum class EBulletFixedParabolaEndParamType : uint8_t
{
	UseDirAsConfig = 0,
	UseSpeedAsConfig = 1,
	UseMaxHeightAsConfig = 2,
	EBulletFixedParabolaEndParamType_MAX = 3
};

// Object: Enum Feature_Arena.EBulletParabolaMoveMode
enum class EBulletParabolaMoveMode : uint8_t
{
	ParabolaMoveMode_FixedParams = 0,
	ParabolaMoveMode_FixedParabolaEnd = 1,
	ParabolaMoveMode_MAX = 2
};

// Object: Enum Feature_Arena.EAblSpawnBulletIntervalType
enum class EAblSpawnBulletIntervalType : uint8_t
{
	Value = 0,
	CalculateBasedOnAttackSpeed = 1,
	EAblSpawnBulletIntervalType_MAX = 2
};

// Object: Enum Feature_Arena.EBulletCircleMoveAttachTargetType
enum class EBulletCircleMoveAttachTargetType : uint8_t
{
	BulletOwner = 0,
	SkillTarget = 1,
	EBulletCircleMoveAttachTargetType_MAX = 2
};

// Object: Enum Feature_Arena.EBulletCircleMoveDir
enum class EBulletCircleMoveDir : uint8_t
{
	Clockwise = 0,
	Counterclockwise = 1,
	EBulletCircleMoveDir_MAX = 2
};

// Object: Enum Feature_Arena.EArenaConditionBulletCheckTarget
enum class EArenaConditionBulletCheckTarget : uint8_t
{
	CheckCaster = 0,
	CheckTarget = 1,
	EArenaConditionBulletCheckTarget_MAX = 2
};

// Object: Enum Feature_Arena.EBulletNoTargetDirType
enum class EBulletNoTargetDirType : uint8_t
{
	UseBulletForward = 0,
	UseOwnerForward = 1,
	EBulletNoTargetDirType_MAX = 2
};

// Object: Enum Feature_Arena.EBulletTargetMode
enum class EBulletTargetMode : uint8_t
{
	BulletTargetMode_UseSkillTarget = 0,
	BulletTargetMode_UseCustomLocation = 1,
	BulletTargetMode_UseAlertLocation = 2,
	BulletTargetMode_UseSkillCaster = 3,
	BulletTargetMode_MAX = 4
};

// Object: Enum Feature_Arena.EMovementSpace
enum class EMovementSpace : uint8_t
{
	World = 0,
	Local = 1,
	Target = 2,
	EMovementSpace_MAX = 3
};

// Object: Enum Feature_Arena.EBulletMoveMode
enum class EBulletMoveMode : uint8_t
{
	NotFollowTarget = 0,
	AutoFollow = 1,
	Parabola = 2,
	CircleMove = 3,
	EBulletMoveMode_MAX = 4
};

// Object: Enum Feature_Arena.EArenaBulletHitObjectTypeEnum
enum class EArenaBulletHitObjectTypeEnum : uint8_t
{
	UnSet = 0,
	Character = 1,
	Env = 2,
	OtherBullet = 3,
	EArenaBulletHitObjectTypeEnum_MAX = 4
};

// Object: Enum Feature_Arena.EArenaBulletDestroyReason
enum class EArenaBulletDestroyReason : uint8_t
{
	ForceDestroy = 0,
	DestroyOnHit = 1,
	DestroyByMoveCondition = 2,
	DestroyByTimelineEndTimer = 3,
	DestroyOnGameDestroy = 4,
	DestroyByHaiYueWorld = 5,
	DestroyOnOwnerDead = 6,
	DestroyOnColaPassive = 7,
	EArenaBulletDestroyReason_MAX = 8
};

// Object: Enum Feature_Arena.EArenaBulletHideTagType
enum class EArenaBulletHideTagType : uint8_t
{
	InValid = 0,
	Arena_Bullet_HideSkill = 1,
	Arena_Bullet_HideLoseView = 2,
	Arena_Bullet_HideInGrass = 3,
	Arena_Bullet_HideWithOwner = 4,
	EArenaBulletHideTagType_MAX = 5
};

// Object: Enum Feature_Arena.EArenaCardLevel
enum class EArenaCardLevel : uint8_t
{
	None = 0,
	Level1 = 1,
	Level2 = 2,
	Level3 = 3,
	EArenaCardLevel_MAX = 4
};

// Object: Enum Feature_Arena.EArenaGameMatchType
enum class EArenaGameMatchType : uint8_t
{
	GameMatchType_MainGame_Rank = 0,
	GameMatchType_MainGame_Rank = 1,
	GameMatchType_MainGame_Rank = 2,
	GameMatchType_MainGame_Relax = 3,
	GameMatchType_MainGame_Relax = 4,
	GameMatchType_MainGame_Relax = 5,
	GameMatchType_BNB = 6,
	GameMatchType_BNB = 7,
	GameMatchType_BNB = 8,
	GameMatchType_Lightning = 9,
	GameMatchType_Lightning = 10,
	GameMatchType_Lightning = 11,
	GameMatchType_BNB_Rank = 12,
	GameMatchType_BNB_Rank2 = 13,
	GameMatchType_BNB_Rank4 = 14,
	GameMatchType_LightningNew = 15,
	GameMatchType_LightningNew = 16,
	GameMatchType_LightningNew = 17,
	GameMatchType_WDXD = 18,
	GameMatchType_DMM = 19,
	GameMatchType_UGCEditor = 20,
	GameMatchType_UGCPlay = 21,
	GameMatchType_MCG1V4 = 22,
	GameMatchType_MCG2V8 = 23,
	GameMatchType_MCGRankedMode = 24,
	GameMatchType_MCG3V9 = 25,
	GameMatchType_MAYCOOP = 26,
	GameMatchType_MAYSPY = 27,
	GameMatchType_MAYINF = 28,
	GameMatchType_GunGame = 29,
	GameMatchType_BioChase = 30,
	GameMatchType_GunGame_TeamMatch = 31,
	GameMatchType_GunGameTDM = 32,
	GameMatchType_JSFC = 33,
	GameMatchType_WQDS = 34,
	GameMatchType_TWMHD = 35,
	GameMatchType_TYCGame = 36,
	GameMatchType_TDGame = 37,
	GameMatchType_TDSmashGame = 38,
	GameMatchType_Roguelike1 = 39,
	GameMatchType_Roguelike2 = 40,
	GameMatchType_Roguelike4 = 41,
	GameMatchType_GunGameKC = 42,
	GameMatchType_COCGame = 43,
	GameMatchType_COCBattleGame = 44,
	GameMatchType_OMDGameNew = 45,
	GameMatchType_OMDGame1 = 46,
	GameMatchType_OMDGame2 = 47,
	GameMatchType_OMDGame4 = 48,
	GameMatchType_OMDGame2_Test = 49,
	GameMatchType_OMDGame4_Test = 50,
	GameMatchType_DDP = 51,
	GameMatchType_DDPT = 52,
	GameMatchType_DDPTeamCompetition = 53,
	GameMatchType_DDPTrial = 54,
	GameMatchType_DDP_Airship = 55,
	GameMatchType_DDP_Mountain = 56,
	GameMatchType_DDB = 57,
	GameMatchType_Arena = 58,
	GameMatchType_Arena_Train = 59,
	GameMatchType_HOK_Train = 60,
	GameMatchType_Arena_3v3 = 61,
	GameMatchType_Arena_3v3_Rank = 62,
	GameMatchType_Arena_HotZone = 63,
	GameMatchType_BS_Team2 = 64,
	GameMatchType_HOK = 65,
	GameMatchType_HOK_Rank = 66,
	GameMatchType_HOK_Infinite = 67,
	GameMatchType_FB = 68,
	GameMatchType_Arena_Max = 69,
	GameMatchType_MAX = 70
};

// Object: Enum Feature_Arena.EArenaTargetTypeOnlyForTaskConfig
enum class EArenaTargetTypeOnlyForTaskConfig : uint8_t
{
	ARENA_TT_Character = 0,
	ARENA_TT_BrokenObject = 1,
	ARENA_TT_Breakable = 2,
	ARENA_TT_Tower = 3,
	ARENA_TT_SleepMonster = 4,
	ARENA_TT_ActiveMonster = 5,
	ARENA_TT_Soldier = 6,
	ARENA_TT_Portal = 7,
	ARENA_TT_CrystalSoldier = 8,
	ARENA_TT_LaunchDevice = 9,
	ARENA_TT_IronChest = 10,
	ARENA_TT_Bullet = 11,
	ARENA_TT_FootBall = 12,
	ARENA_TT_StaticSummonCharacter = 13,
	ARENA_TT_MoveSummonCharacter = 14,
	ARENA_TT_Mount = 15,
	ARENA_TT_Harvester = 16,
	ARENA_TT_MAX = 17
};

// Object: Enum Feature_Arena.EArenaActorTypeOnlyForTaskConfig
enum class EArenaActorTypeOnlyForTaskConfig : uint8_t
{
	ARENA_AT_Character = 0,
	ARENA_AT_Bullet = 1,
	ARENA_AT_MagicField = 2,
	ARENA_AT_BrokenObject = 3,
	ARENA_AT_Breakable = 4,
	ARENA_AT_Potion = 5,
	ARENA_AT_Card = 6,
	ARENA_AT_ReviveCircle = 7,
	ARENA_AT_PoisonCircle = 8,
	ARENA_AT_Tower = 9,
	ARENA_AT_Monster = 10,
	ARENA_AT_Soldier = 11,
	ARENA_AT_Portal = 12,
	ARENA_AT_CrystalSoldier = 13,
	ARENA_AT_LaunchDevice = 14,
	ARENA_AT_HotZoneCircle = 15,
	ARENA_AT_IronChest = 16,
	ARENA_AT_FootBall = 17,
	ARENA_AT_FootBallGoal = 18,
	ARENA_AT_SummonCharacter = 19,
	ARENA_AT_Mount = 20,
	ARENA_AT_Harvester = 21,
	ARENA_AT_MAX = 22
};

// Object: Enum Feature_Arena.EArenaCharacterMotion3CType
enum class EArenaCharacterMotion3CType : uint8_t
{
	BaseArena = 0,
	BrawlStar = 1,
	EArenaCharacterMotion3CType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaCharacterMovementType
enum class EArenaCharacterMovementType : uint8_t
{
	None = 0,
	Normal = 1,
	RootMotion = 2,
	EArenaCharacterMovementType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaTrusteeType
enum class EArenaTrusteeType : uint8_t
{
	EArenaTrusteeType_default = 0,
	EArenaTrusteeType_ActionSkill = 1,
	EArenaTrusteeType_AIRobotControl = 2,
	EArenaTrusteeType_Max = 3
};

// Object: Enum Feature_Arena.EArenaC2MoveSignificanceType
enum class EArenaC2MoveSignificanceType : uint8_t
{
	EArenaC2MoveSignificanceType_Invalid = 0,
	EArenaC2MoveSignificanceType_Full = 1,
	EArenaC2MoveSignificanceType_IgnoreFindFlore = 2,
	EArenaC2MoveSignificanceType_LerpPosition = 3,
	EArenaC2MoveSignificanceType_SetPosition = 4,
	EArenaC2MoveSignificanceType_MAX = 5
};

// Object: Enum Feature_Arena.EArenaCarPassengerState
enum class EArenaCarPassengerState : uint8_t
{
	Rider = 0,
	Enemy1 = 1,
	Enemy2 = 2,
	EArenaCarPassengerState_MAX = 3
};

// Object: Enum Feature_Arena.EArenaMotionTurnSection
enum class EArenaMotionTurnSection : uint8_t
{
	Arena_TurnTo_StateOnEnter = 0,
	Arena_TurnTo_StateUpDate = 1,
	Arena_TurnTo_StateExit = 2,
	Arena_TurnTo_MAX = 3
};

// Object: Enum Feature_Arena.EArenaPushOtherBit
enum class EArenaPushOtherBit : uint8_t
{
	Invalid = 0,
	SmallLuoLuo = 1,
	EArenaPushOtherBit_MAX = 2
};

// Object: Enum Feature_Arena.EArenaVoiceCustomizedEventType
enum class EArenaVoiceCustomizedEventType : uint8_t
{
	Character_Into_Water_Name = 0,
	Character_Into_Water_Name_3p = 1,
	Character_Out_Of_Water_Name = 2,
	Character_Out_Of_Water_Name_3p = 3,
	Character_MAX = 4
};

// Object: Enum Feature_Arena.EArenaVoiceType
enum class EArenaVoiceType : uint8_t
{
	Character_HeroSelect_Name = 0,
	Character_StopHeroSelect_Name = 1,
	Character_Born_Name = 2,
	Character_Revive_Name = 3,
	Character_Attack_Name = 4,
	Character_Skill1_Name = 5,
	Character_Skill2_Name = 6,
	Character_Ultimate_Name = 7,
	Character_Attack_3P_Name = 8,
	Character_Skill1_3P_Name = 9,
	Character_Skill2_3P_Name = 10,
	Character_Ultimate_3P_Name = 11,
	Character_Defeated_Name = 12,
	Character_Defeated_3P_Name = 13,
	Character_Kill_Name = 14
};

// Object: Enum Feature_Arena.EArenaCombatEventType
enum class EArenaCombatEventType : uint8_t
{
	Invalid = 0,
	Damage = 1,
	SKill = 2,
	Buff = 3,
	MagicField = 4,
	BrokenObject = 5,
	EArenaCombatEventType_MAX = 6
};

// Object: Enum Feature_Arena.EArenaAblFixedCameraLocType
enum class EArenaAblFixedCameraLocType : uint8_t
{
	SelfLoc = 0,
	SkillTargetLoc = 1,
	EArenaAblFixedCameraLocType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaUIDynamicMountPosType
enum class EArenaUIDynamicMountPosType : uint8_t
{
	Up = 0,
	Down = 1,
	EArenaUIDynamicMountPosType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaSkillForbidType
enum class EArenaSkillForbidType : uint8_t
{
	InValid = 0,
	NotCareNormalAttack = 1,
	OnlyCareNormalAttack = 2,
	EArenaSkillForbidType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaUIType
enum class EArenaUIType : uint8_t
{
	UI_Arena_ShowDamageTip = 0,
	UI_Arena_ShowDamageItem = 1,
	UI_HOK_ShowDamageTip = 2,
	UI_HOK_ShowDamageItem = 3,
	UI_Arena_NameBar = 4,
	UI_Arena_AttackStackCount = 5,
	UI_Arena_ChantingProgress = 6,
	UI_Arena_CommonAttributeBarContain = 7,
	UI_Arena_LuoPuXia_Progress = 8,
	UI_Arena_CommonAttributeBar = 9,
	UI_Arena_RedFox_GoldCoinList = 10,
	UI_Arena_RedFox_GoldCoin_EnergyBar = 11,
	UI_Arena_Ninja_PowerMarkList = 12,
	UI_Arena_Ninja_PowerMark = 13,
	UI_Arena_CampIcon = 14,
	UI_Arena_GongBenWuZang_Enemy = 15,
	UI_Arena_NameBarUpElement = 16,
	UI_Arena_CommonTimeBarContain = 17,
	UI_Arena_NameBarRightElement = 18,
	UI_Arena_LongPressSkill_Progress = 19,
	UI_Arena_ChargeSkill_Progress = 20,
	UI_Arena_Summon_SkillIconElement = 21,
	UI_Arena_Summon_SkillIconItem = 22,
	UI_Arena_NameBar_Monster = 23,
	UI_Arena_TimeChargeStackCount = 24,
	UI_Arena_MakeEnemy = 25,
	UI_Arena_XiaoMai_Progress = 26,
	UI_Arena_HeadLockEnemy = 27,
	UI_Arena_LockEnemy = 28,
	UI_Arena_SkillHeroHead = 29,
	UI_Arena_NameBar_AttackStackCountTow = 30,
	UI_Arena_NameBar_AbnormalProgress = 31,
	UI_Arena_TeammateHead = 32,
	UI_ArenaSkillButton = 50,
	UI_Arena_SkillPointView = 51,
	UI_MAX = 52
};

// Object: Enum Feature_Arena.EArenaIndicatorControlType
enum class EArenaIndicatorControlType : uint8_t
{
	ControlType_None = 1,
	ControlType_Direction = 2,
	ControlType_Target = 3,
	ControlType_LockTarget = 4,
	ControlType_MAX = 5
};

// Object: Enum Feature_Arena.EArenaCharacterBindInfoType
enum class EArenaCharacterBindInfoType : uint8_t
{
	Character = 0,
	Skill = 1,
	ShareProfitSlot = 2,
	ShareProfitInfo = 3,
	DuoLiYaCDToken = 4,
	EArenaCharacterBindInfoType_MAX = 5
};

// Object: Enum Feature_Arena.EBulletAppearanceBPPlayAnimationState
enum class EBulletAppearanceBPPlayAnimationState : uint8_t
{
	ChangeAnimStateInt = 0,
	PlayAnimSequenceAsMontage = 1,
	EBulletAppearanceBPPlayAnimationState_MAX = 2
};

// Object: Enum Feature_Arena.EArenaPlantFlowerGainState
enum class EArenaPlantFlowerGainState : uint8_t
{
	Normal = 0,
	Silver = 1,
	Gold = 2,
	EArenaPlantFlowerGainState_MAX = 3
};

// Object: Enum Feature_Arena.EArenaPlantFlowerState
enum class EArenaPlantFlowerState : uint8_t
{
	Seed = 0,
	Grow = 1,
	Flower_Protected = 2,
	Flower_Steal = 3,
	EArenaPlantFlowerState_MAX = 4
};

// Object: Enum Feature_Arena.EArenaSelectType
enum class EArenaSelectType : uint8_t
{
	Normal = 0,
	ForceLock = 1,
	TryForceLock = 2,
	SkillLock = 3,
	AttackFollow = 4,
	EArenaSelectType_MAX = 5
};

// Object: Enum Feature_Arena.EArenaLockEffectType
enum class EArenaLockEffectType : uint8_t
{
	None = 0,
	ForceLockCharacter = 1,
	ForceLockMonsterBuilding = 2,
	TryForceLockCharacter = 3,
	TryForceLockMonsterBuilding = 4,
	AttackFollowCharacter = 5,
	AttackFollowMonsterBuilding = 6,
	EArenaLockEffectType_MAX = 7
};

// Object: Enum Feature_Arena.ESpellCastingRangeType
enum class ESpellCastingRangeType : uint8_t
{
	Common = 0,
	HeadLock = 1,
	IndicatorAbsorb = 2,
	AllEnemyCharacters = 3,
	AllVisibleEnemyCharacters = 4,
	ForceLock = 5,
	ESpellCastingRangeType_MAX = 6
};

// Object: Enum Feature_Arena.EArenaHideNameBarTag
enum class EArenaHideNameBarTag : uint8_t
{
	ARENA_Hide_NameBar_Invalid = 0,
	ARENA_Hide_NameBar_Invisible = 1,
	ARENA_Hide_NameBar_Dead = 2,
	ARENA_Hide_NameBar_Ninja = 3,
	ARENA_Hide_NameBar_General = 4,
	ARENA_Hide_NameBar_MAX = 5
};

// Object: Enum Feature_Arena.ArenaSkillBtnTriggerType
enum class EArenaSkillBtnTriggerType : uint8_t
{
	Common = 0,
	NormalAttackClick = 1,
	NormalAttackDrag = 2,
	LongPressChangeSkill = 3,
	ArenaSkillBtnTriggerType_MAX = 4
};

// Object: Enum Feature_Arena.ArenaCommonBrushVisibleType
enum class EArenaCommonBrushVisibleType : uint8_t
{
	Invalid = 0,
	Visible = 1,
	HalfVisible = 2,
	InVisible = 3,
	ArenaCommonBrushVisibleType_MAX = 4
};

// Object: Enum Feature_Arena.EArenaBuffSnapshotFinalModifyCommon
enum class EArenaBuffSnapshotFinalModifyCommon : uint8_t
{
	Arena_BSFM_None = 0,
	Arena_BSFM_HitNum = 1,
	Arena_BSFM_ChargeTime = 2,
	Arena_BSFM_Speed = 3,
	Arena_BSFM_HpLost = 4,
	Arena_BSFM_MAX = 5
};

// Object: Enum Feature_Arena.EArenaBuffTargetTypeCommon
enum class EArenaBuffTargetTypeCommon : uint8_t
{
	Arena_Target_Attacker = 0,
	Arena_Target_Defender = 1,
	Arena_Target_MAX = 2
};

// Object: Enum Feature_Arena.EArenaUnitStateType
enum class EArenaUnitStateType : uint8_t
{
	LocalUnitState = 0,
	ServerUnitState = 1,
	LocalActiveForbid = 2,
	ServerActiveForbid = 3,
	LocalPassiveForbid = 4,
	ServerPassiveForbid = 5,
	EArenaUnitStateType_MAX = 6
};

// Object: Enum Feature_Arena.ERotLockType
enum class ERotLockType : uint8_t
{
	World = 0,
	IndicatorDirection = 1,
	FirstFrameRelative = 2,
	Type3 = 3,
	Type4 = 4,
	Type5 = 5,
	ERotLockType_MAX = 6
};

// Object: Enum Feature_Arena.EArenaIllusionLocationType
enum class EArenaIllusionLocationType : uint8_t
{
	None = 0,
	Self = 1,
	Target = 2,
	Range = 3,
	AllAllies = 4,
	AllEnemies = 5,
	Indicator = 6,
	EArenaIllusionLocationType_MAX = 7
};

// Object: Enum Feature_Arena.EArenaIllusionControlType
enum class EArenaIllusionControlType : uint8_t
{
	None = 0,
	RemoteControl = 1,
	BTAIControl = 2,
	TimelineControl = 3,
	EArenaIllusionControlType_MAX = 4
};

// Object: Enum Feature_Arena.EArenaBuffIntervalOPType
enum class EArenaBuffIntervalOPType : uint8_t
{
	Start = 0,
	End = 1,
	ForceRestart = 2,
	EArenaBuffIntervalOPType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaParticleContinuousFaceToTargetEnum
enum class EArenaParticleContinuousFaceToTargetEnum : uint8_t
{
	Bullet = 0,
	BulletOwner = 1,
	EArenaParticleContinuousFaceToTargetEnum_MAX = 2
};

// Object: Enum Feature_Arena.EArenaCommonTargetType
enum class EArenaCommonTargetType : uint8_t
{
	None = 0,
	Self = 1,
	Target = 2,
	EArenaCommonTargetType_MAX = 3
};

// Object: Enum Feature_Arena.ESelectTargetOrderEnum
enum class ESelectTargetOrderEnum : uint8_t
{
	Random = 0,
	Left2Right = 1,
	Right2Left = 2,
	ESelectTargetOrderEnum_MAX = 3
};

// Object: Enum Feature_Arena.EArenaHitHarmableType
enum class EArenaHitHarmableType : uint8_t
{
	Normal = 0,
	TargetOnly = 1,
	WithSpecificBuff = 2,
	EArenaHitHarmableType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaAirBombingLookAtCenterType
enum class EArenaAirBombingLookAtCenterType : uint8_t
{
	BombingZoneCenter = 0,
	LevelCenter = 1,
	EArenaAirBombingLookAtCenterType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaShapeComponentType
enum class EArenaShapeComponentType : uint8_t
{
	Invalid = 0,
	Sphere = 1,
	Box = 2,
	Capsule = 3,
	EArenaShapeComponentType_MAX = 4
};

// Object: Enum Feature_Arena.EArenaHitShakeHpBarPreset
enum class EArenaHitShakeHpBarPreset : uint8_t
{
	Custom = 0,
	Low = 1,
	Medium = 2,
	High = 3,
	EArenaHitShakeHpBarPreset_MAX = 4
};

// Object: Enum Feature_Arena.EArenaSkillCastTarget
enum class EArenaSkillCastTarget : uint8_t
{
	None = 0,
	ToTarget = 1,
	EArenaSkillCastTarget_MAX = 2
};

// Object: Enum Feature_Arena.EArenaSkillCastSource
enum class EArenaSkillCastSource : uint8_t
{
	FromMaster = 0,
	FromSelf = 1,
	EArenaSkillCastSource_MAX = 2
};

// Object: Enum Feature_Arena.EArenaAttrModifyCheckIntervalType
enum class EArenaAttrModifyCheckIntervalType : uint8_t
{
	Arena_AttrModifyCheckInterval_Replace = 0,
	Arena_AttrModifyCheckInterval_Multiple = 1,
	Arena_AttrModifyCheckInterval_Add = 2,
	Arena_AttrModifyCheckInterval_MAX = 3
};

// Object: Enum Feature_Arena.EArenaSetVisibilityFlagType
enum class EArenaSetVisibilityFlagType : uint16_t
{
	ARENA_VisFlag_UpdateAllAttachedChildrenVisibility = 0,
	ARENA_VisFlag_AllFlags = 255,
	ARENA_VisFlag_MAX = 256
};

// Object: Enum Feature_Arena.ArenaOutlineFlagPriorityType
enum class EArenaOutlineFlagPriorityType : uint8_t
{
	ARENA_Outline_Default = 0,
	ARENA_Outline_BS_Grass = 10,
	ARENA_Outline_Hit = 20,
	ARENA_Outline_MAX = 21
};

// Object: Enum Feature_Arena.EArenaVisiblityFlagPriorityType
enum class EArenaVisiblityFlagPriorityType : uint8_t
{
	ARENA_VisPri_Default = 0,
	ARENA_VisPri_HighLight = 10,
	ARENA_VisPri_Bloodrage = 11,
	ARENA_VisPri_Item = 20,
	ARENA_VisPri_Skill = 30,
	ARENA_NinjaIllusion_Default = 31,
	ARENA_VisPri_Grass = 40,
	ARENA_VisPri_Grass_Hit = 50,
	ARENA_VisPri_Skill_Invisible = 59,
	ARENA_VisPri_Skill_Lan = 60,
	ARENA_VisPri_YaoAttachShow = 70,
	ARENA_VisPri_Show = 80,
	ARENA_VisPri_Frozen = 81,
	ARENA_VisPri_Soul = 82,
	ARENA_VisPri_GlodenBody = 90,
	ARENA_VisPri_SystemInvisible_ShowNameBar = 91,
	ARENA_VisPri_SystemInvisible_LoseView = 92,
	ARENA_VisPri_RandomEvent_BossInVisible = 93,
	ARENA_VisPri_SystemInvisible = 100,
	ARENA_VisPri_SystemAndParticleInvisible = 110,
	ARENA_MAX = 111
};

// Object: Enum Feature_Arena.EArenaBuffAbnormalStatus
enum class EArenaBuffAbnormalStatus : uint8_t
{
	Arena_BAS_Unstoppable = 0,
	Arena_BAS_Invincible = 1,
	Arena_BAS_UnSelectable = 2,
	Arena_BAS_Dizzy = 3,
	Arena_BAS_KnockBack = 4,
	Arena_BAS_KnockUp = 5,
	Arena_BAS_Invisible = 6,
	Arena_BAS_Grass_Invisible = 7,
	Arena_BAS_Frozen = 8,
	Arena_BAS_Expose = 9,
	Arena_BAS_Abstract = 10,
	Arena_BAS_SlowDown = 11,
	Arena_BAS_MAX = 12
};

// Object: Enum Feature_Arena.EArenaFilterType
enum class EArenaFilterType : uint8_t
{
	Random = 0,
	DistanceCloser = 1,
	DistanceFarther = 2,
	BloodLess = 3,
	BloodMore = 4,
	BloodPercentageMore = 5,
	BloodPercentageLess = 6,
	EArenaFilterType_MAX = 7
};

// Object: Enum Feature_Arena.EArenaHitFlag
enum class EArenaHitFlag : uint8_t
{
	None = 0,
	CloseDmgMsg = 1,
	CloseDmgCount = 2,
	EArenaHitFlag_MAX = 3
};

// Object: Enum Feature_Arena.ECharacterBaseViewType
enum class ECharacterBaseViewType : uint8_t
{
	Default = 0,
	HighLightBody = 10,
	Bloodrage = 11,
	ItemInVisiable = 20,
	BaseViewType1 = 25,
	HalfVisiable = 30,
	NinjaIllusionMaterial = 31,
	BaseViewType2 = 35,
	GrassInVisiable = 40,
	BaseViewType3 = 45,
	GrassHitVisible = 50,
	BaseViewType4 = 55,
	SkillInVisible_Lan = 60,
	BaseViewType5 = 65,
	YaoAttachShowBody = 70,
	BaseViewType6 = 75,
	ShowBody = 80,
	FrozenBody = 81,
	SoulBody = 82,
	BaseViewType7 = 85,
	GlodenBody = 90,
	InVisible_ShowNameBar = 91,
	InVisible_LoseView = 92,
	RandomEvent_BossInVisible = 93,
	BaseViewType8 = 95,
	SystemInVisible = 100,
	SystemAndParticleInVisible = 110,
	ECharacterBaseViewType_MAX = 111
};

// Object: Enum Feature_Arena.ECharacterBaseViewTypeOld
enum class ECharacterBaseViewTypeOld : uint8_t
{
	Default = 0,
	HalfVisiable = 1,
	GrassInVisiable = 2,
	ItemInVisiable = 3,
	GlodenBody = 4,
	HighLightBody = 5,
	ShowBody = 6,
	GrassHitVisible = 7,
	SkillInVisible_Lan = 8,
	SystemInVisible = 9,
	YaoAttachShowBody = 10,
	Bloodrage = 11,
	FrozenBody = 12,
	SoulBody = 13,
	InVisible_ShowNameBar = 14,
	InVisible_LoseView = 15,
	RandomEvent_BossInVisible = 16,
	SystemAndParticleInVisible = 17,
	NinjaIllusionMaterial = 18,
	BaseViewType1 = 20,
	BaseViewType2 = 21,
	BaseViewType3 = 22,
	BaseViewType4 = 23,
	ECharacterBaseViewTypeOld_MAX = 24
};

// Object: Enum Feature_Arena.EArenaCheckTagsOp
enum class EArenaCheckTagsOp : uint8_t
{
	NotInUse = 0,
	HaveAll = 1,
	HaveAny = 2,
	MissAll = 3,
	MissAny = 4,
	EArenaCheckTagsOp_MAX = 5
};

// Object: Enum Feature_Arena.EArenaSkillSpecialTags
enum class EArenaSkillSpecialTags : uint8_t
{
	ASST_NotInUse = 0,
	ASST_NotInterruptByJinShen = 1,
	ASST_MAX = 2
};

// Object: Enum Feature_Arena.EArenaWaterAreaSpawnPos
enum class EArenaWaterAreaSpawnPos : uint8_t
{
	CasterPos = 1,
	ThrowPos = 2,
	EArenaWaterAreaSpawnPos_MAX = 3
};

// Object: Enum Feature_Arena.EArenaPlayParticleRelativeDirType
enum class EArenaPlayParticleRelativeDirType : uint8_t
{
	SocketDir = 1,
	SkillStartCasterDir = 2,
	SkillDir = 3,
	CasterDir = 4,
	SpawnToTargetDir = 5,
	ConfigDir = 6,
	TargetDirTick = 7,
	SpawnToCasterDir = 8,
	CachePositionDirTick = 11,
	ConnectTargetAndSource = 12,
	SelfDirTick = 13,
	EArenaPlayParticleRelativeDirType_MAX = 14
};

// Object: Enum Feature_Arena.EArenaSummonSpawnPos
enum class EArenaSummonSpawnPos : uint8_t
{
	ThrowPos = 1,
	CasterPos = 2,
	TargetPos = 3,
	EArenaSummonSpawnPos_MAX = 4
};

// Object: Enum Feature_Arena.EArenaAbnormalMotionState
enum class EArenaAbnormalMotionState : uint8_t
{
	AbnormalMotion_Chaos = 1,
	AbnormalMotion_Enchant = 2,
	AbnormalMotion_MAX = 3
};

// Object: Enum Feature_Arena.EArenaPlayParticleRelativePosType
enum class EArenaPlayParticleRelativePosType : uint8_t
{
	SocketPos = 1,
	SkillStartPos = 2,
	ThrowPos = 3,
	CasterPos = 4,
	TargetPos = 5,
	TargetSkillStartPos = 6,
	FloorPosition = 7,
	TargetPosAttach = 8,
	FloorPositionStart = 9,
	TargetFloorPos = 10,
	CachePositionTick = 11,
	ConnectTargetAndSource = 12,
	HitLocation = 13,
	FollowSocketPos = 14,
	ConfigPos = 15,
	TargetFloorPositionStart = 16,
	EArenaPlayParticleRelativePosType_MAX = 17
};

// Object: Enum Feature_Arena.EArenaTargetSelectionStrategy
enum class EArenaTargetSelectionStrategy : uint8_t
{
	ATSS_Distance = 0,
	ATSS_HPValue = 1,
	ATSS_HPPercentage = 2,
	ATSS_MAX = 3
};

// Object: Enum Feature_Arena.ETurnToTarget
enum class ETurnToTarget : uint8_t
{
	Instigator = 0,
	SkillDir = 1,
	Joystick = 2,
	LevelCenter = 3,
	FollowingIndicator = 4,
	ParentJoyStick = 5,
	ETurnToTarget_MAX = 6
};

// Object: Enum Feature_Arena.EArenaFakeWallType
enum class EArenaFakeWallType : uint8_t
{
	CircleWall = 0,
	LineWall = 1,
	CylinderWall = 2,
	EArenaFakeWallType_MAX = 3
};

// Object: Enum Feature_Arena.ESkillIndicatorLockType
enum class ESkillIndicatorLockType : uint8_t
{
	Common = 0,
	SkillLock = 1,
	SkillCast = 2,
	SkillHit = 3,
	OverDistance = 4,
	Death = 5,
	TryForceLock = 6,
	ESkillIndicatorLockType_MAX = 7
};

// Object: Enum Feature_Arena.EArenaTypeRPCType
enum class EArenaTypeRPCType : uint8_t
{
	ToServerWithType_Reliable = 0,
	ToClientWithType_Reliable = 1,
	BroadcastWithType_Reliable = 2,
	ToServerWithType_Unreliable = 3,
	ToClientWithType_Unreliable = 4,
	BroadcastWithType_Unreliable = 5,
	EArenaTypeRPCType_MAX = 6
};

// Object: Enum Feature_Arena.EArenaRPCType
enum class EArenaRPCType : uint8_t
{
	ToServer_Reliable = 0,
	ToClient_Reliable = 1,
	Broadcast_Reliable = 2,
	ToServer_Unreliable = 3,
	ToClient_Unreliable = 4,
	Broadcast_Unreliable = 5,
	EArenaRPCType_MAX = 6
};

// Object: Enum Feature_Arena.EArenaCheckTargetBehindWallType
enum class EArenaCheckTargetBehindWallType : uint8_t
{
	AttackerToTarget = 0,
	AttackDirection = 1,
	EArenaCheckTargetBehindWallType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaHitShapeModifier
enum class EArenaHitShapeModifier : uint8_t
{
	Invalid = 0,
	C_Height = 1,
	C_Inner = 2,
	C_Outer = 3,
	F_RelativeDir = 4,
	F_Height = 5,
	F_Inner = 6,
	F_Outer = 7,
	F_Angle = 8,
	R_RelativeDir = 9,
	R_Height = 10,
	R_Length = 11,
	R_Width = 12,
	S_Radius = 13,
	CF_Outer = 14,
	CF_Inner = 15,
	CF_Height = 16,
	SCALE_Distance = 17,
	EArenaHitShapeModifier_MAX = 18
};

// Object: Enum Feature_Arena.ERelativeDirType
enum class ERelativeDirType : uint8_t
{
	CasterFaceDir = 0,
	SkillDir = 1,
	IndicatorDir = 2,
	CasterToTarget = 3,
	HeroSkillSpawnActorFaceDir = 4,
	ERelativeDirType_MAX = 5
};

// Object: Enum Feature_Arena.EAttackRegionHitBackType
enum class EAttackRegionHitBackType : uint8_t
{
	Atk2Dfs = 0,
	RegionCenter2Dfs = 1,
	SkillDir = 2,
	ToRegionCenter = 3,
	Bullet2Dfs = 4,
	EAttackRegionHitBackType_MAX = 5
};

// Object: Enum Feature_Arena.ERelativePosType
enum class ERelativePosType : uint8_t
{
	Self = 0,
	SelfPos = 1,
	Target = 2,
	Throw = 3,
	TargetStartPos = 4,
	BombingZoneCenter = 5,
	HitLocation = 6,
	HeroSkillSpawnActorLocation = 7,
	TaskStartPos = 8,
	ERelativePosType_MAX = 9
};

// Object: Enum Feature_Arena.EArenaHitShapeDynamicType
enum class EArenaHitShapeDynamicType : uint8_t
{
	Trail = 0,
	Ring = 1,
	EArenaHitShapeDynamicType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaShapeType
enum class EArenaShapeType : uint8_t
{
	Box = 0,
	Sphere = 1,
	Cylinder = 2,
	Sector = 4,
	EArenaShapeType_MAX = 5
};

// Object: Enum Feature_Arena.EArenaSetBuffTargetType
enum class EArenaSetBuffTargetType : uint16_t
{
	None = 0,
	Self = 1,
	Target = 2,
	AttachTo = 3,
	IllusionOwner = 4,
	Reserved = 255,
	EArenaSetBuffTargetType_MAX = 256
};

// Object: Enum Feature_Arena.ECharacterMoveDirectionMode
enum class ECharacterMoveDirectionMode : uint8_t
{
	FaceDirection = 0,
	IndicatorDirection = 1,
	RealFaceDirection = 2,
	ECharacterMoveDirectionMode_MAX = 3
};

// Object: Enum Feature_Arena.ECharacterMoveMode
enum class ECharacterMoveMode : uint8_t
{
	FixedDistance = 0,
	HitAir = 1,
	Teleport = 2,
	TeleportToBullet = 3,
	MoveToSkillTarget = 4,
	TeleportToMarkedPosition = 5,
	MoveToIndicatorPos = 6,
	MarkPosition = 7,
	TeleportToSkillTarget = 8,
	TeleportToHome = 9,
	TeleportToTarget = 10,
	HitAirHOKMonster = 11,
	MoveToCachePos = 12,
	PortalSpline = 13,
	TeleportToCrystal = 14,
	PortalMove = 15,
	BlinkForward = 16,
	LaunchMove = 17,
	LaunchSpline = 18,
	DynamicSpline = 19,
	TeleportToConfigPosition = 20,
	TeleportToIndicatorPos = 21,
	ECharacterMoveMode_MAX = 22
};

// Object: Enum Feature_Arena.EArenaNumberCheckOp
enum class EArenaNumberCheckOp : uint8_t
{
	NotInUse = 0,
	Above = 1,
	AboveAndEqual = 2,
	Below = 3,
	BelowAndEqual = 4,
	EArenaNumberCheckOp_MAX = 5
};

// Object: Enum Feature_Arena.EArenaMovementTaskType
enum class EArenaMovementTaskType : uint8_t
{
	NormalMove = 0,
	TargetPointMove = 1,
	TargetActorMove = 2,
	AbsolutePathMove = 3,
	EArenaMovementTaskType_MAX = 4
};

// Object: Enum Feature_Arena.ECharacterMoveSpline
enum class ECharacterMoveSpline : uint8_t
{
	Normal = 0,
	LeftHalfCircle = 1,
	RightHalfCircle = 2,
	ECharacterMoveSpline_MAX = 3
};

// Object: Enum Feature_Arena.ECharacterMoveCurve
enum class ECharacterMoveCurve : uint8_t
{
	Linear = 0,
	QuadIn = 1,
	QuadOut = 2,
	QuadInOut = 3,
	CubicIn = 4,
	CubicOut = 5,
	CubicInOut = 6,
	ECharacterMoveCurve_MAX = 7
};

// Object: Enum Feature_Arena.EArenaDetailedGameType
enum class EArenaDetailedGameType : uint8_t
{
	DGT_PublicGame = 0,
	DGT_3V3 = 1,
	DGT_HOK = 2,
	DGT_BS = 3,
	DGT_HZ = 4,
	DGT_FootBall = 5,
	DGT_Training = 6,
	DGT_Training_HOK = 7,
	DGT_HOK_AllMid = 8,
	DGT_BackUp2 = 9,
	DGT_MAX = 10
};

// Object: Enum Feature_Arena.EArenaDynamicParticleType
enum class EArenaDynamicParticleType : uint8_t
{
	DynamicTrailParticle = 0,
	DynamicChainParticleWithTarget = 1,
	EArenaDynamicParticleType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaGlobalCheatFlag
enum class EArenaGlobalCheatFlag : uint8_t
{
	Default = 0,
	EArenaGlobalCheatFlag_MAX = 1
};

// Object: Enum Feature_Arena.EArenaCarState
enum class EArenaCarState : uint8_t
{
	None = 0,
	Driver = 1,
	Rider = 2,
	Enemy = 3,
	Harvester = 4,
	EArenaCarState_MAX = 5
};

// Object: Enum Feature_Arena.EArenaHarvesterCarRunningState
enum class EArenaHarvesterCarRunningState : uint8_t
{
	None = 0,
	DriveCar = 1,
	Idle = 2,
	AutoHit = 3,
	End = 4,
	EArenaHarvesterCarRunningState_MAX = 5
};

// Object: Enum Feature_Arena.EArenaIndicatorEffectType
enum class EArenaIndicatorEffectType : uint8_t
{
	None = 0,
	BulletPos = 1,
	BulletTrail = 2,
	Bomb = 3,
	CarTrail = 4,
	EArenaIndicatorEffectType_MAX = 5
};

// Object: Enum Feature_Arena.EArenaIndicatorType
enum class EArenaIndicatorType : uint8_t
{
	SelfCircle = 1,
	LineDirection = 2,
	TargetCirclePos = 3,
	LineAddCircleCircleDirection = 4,
	RangeDirection = 5,
	RangeTarget = 6,
	MultipleCrossLineTargetPos = 7,
	MultipleLineDirection = 8,
	SelfSquare = 9,
	TargetSquarePos = 10,
	NormalAttackTarget = 11,
	RingTrajectory = 12,
	RingRange = 13,
	Hidden = 14,
	LineAndMultiLine = 15,
	XiangYuLineDirection = 16,
	LineDirectionNoRange = 17,
	CircleAddLine = 18,
	Custom = 100,
	Custom = 101,
	Custom = 102,
	Custom = 103,
	Custom = 104,
	EArenaIndicatorType_MAX = 105
};

// Object: Enum Feature_Arena.ELanSkillNameBarType
enum class ELanSkillNameBarType : uint8_t
{
	None = 0,
	Initialize = 1,
	Check = 2,
	ELanSkillNameBarType_MAX = 3
};

// Object: Enum Feature_Arena.ENameNegativeFlag
enum class ENameNegativeFlag : uint8_t
{
	Flag_None = 0,
	Flag_Visible = 1,
	Flag_PlayerInfoIsNil = 2,
	Flag_HasAbnormalTag = 3,
	Flag_HideNameBarTag = 4,
	Flag_InGrass = 5,
	Flag_NotInSubgame = 6,
	Flag_TrainingDefaultHide = 7,
	Flag_MAX = 8
};

// Object: Enum Feature_Arena.EStateBottomFlag
enum class EStateBottomFlag : uint8_t
{
	InProtectViewRegion = 1,
	InGrass = 2,
	WeakNet = 4,
	Offline = 8,
	HostingByAI = 16,
	EStateBottomFlag_MAX = 17
};

// Object: Enum Feature_Arena.ERoleHostUIType
enum class ERoleHostUIType : uint8_t
{
	ERoleHostUIType = 1,
	ERoleHostUIType = 2,
	ERoleHostUIType = 3,
	ERoleHostUIType_MAX = 4
};

// Object: Enum Feature_Arena.ENameBarSideType
enum class ENameBarSideType : uint8_t
{
	Self = 1,
	Teammate = 2,
	Enemy = 3,
	ENameBarSideType_MAX = 4
};

// Object: Enum Feature_Arena.EArenaTaskType
enum class EArenaTaskType : uint8_t
{
	Default = 0,
	Buff = 1,
	Attr = 2,
	EArenaTaskType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaNetworkTaskPriority
enum class EArenaNetworkTaskPriority : uint8_t
{
	Immediate = 0,
	High = 1,
	Middle = 2,
	Low = 3,
	EArenaNetworkTaskPriority_MAX = 4
};

// Object: Enum Feature_Arena.EArenaOverseaGuideNPCSkillType
enum class EArenaOverseaGuideNPCSkillType : uint8_t
{
	ASS_SkillOne = 0,
	ASS_SkillTwo = 1,
	ASS_SkillUltimate = 2,
	ASS_MAX = 3
};

// Object: Enum Feature_Arena.EArenaOverseaGuideNPCAIState
enum class EArenaOverseaGuideNPCAIState : uint8_t
{
	RangeMove = 0,
	Performance = 1,
	Free = 2,
	FriendFollow = 3,
	EArenaOverseaGuideNPCAIState_MAX = 4
};

// Object: Enum Feature_Arena.EArenaParticleVisibilityFlag
enum class EArenaParticleVisibilityFlag : uint8_t
{
	Skill = 1,
	AnimNode = 2,
	EArenaParticleVisibilityFlag_MAX = 3
};

// Object: Enum Feature_Arena.EArenaPathFollowingStatus
enum class EArenaPathFollowingStatus : uint8_t
{
	Idle = 0,
	Waiting = 1,
	Moving = 2,
	Stuck = 3,
	Paused = 4,
	EArenaPathFollowingStatus_MAX = 5
};

// Object: Enum Feature_Arena.EArenaPathFindingType
enum class EArenaPathFindingType : uint8_t
{
	Sync = 0,
	Frame = 1,
	Async = 2,
	EArenaPathFindingType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaCustomPerfType
enum class EArenaCustomPerfType : uint8_t
{
	Unknown = 0,
	ParticleDrawCalls = 1,
	PlayerNum = 2,
	DownQualityFlag = 3,
	UpQualityFlag = 4,
	EArenaCustomPerfType_MAX = 5
};

// Object: Enum Feature_Arena.EArenaPhantomType
enum class EArenaPhantomType : uint8_t
{
	None = 0,
	DianWei_PhantomType = 1,
	Card_PhantomType = 2,
	EArenaPhantomType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaLoadPathModel
enum class EArenaLoadPathModel : uint8_t
{
	One_LoadPath = 0,
	Array_LoadPath = 1,
	Map_LoadPath = 2,
	EArenaLoadPathModel_MAX = 3
};

// Object: Enum Feature_Arena.EArenaLoadModel
enum class EArenaLoadModel : uint8_t
{
	Frame_Loading = 0,
	Immediately_Loading = 1,
	Custom_Frame_Loading = 2,
	EArenaLoadModel_MAX = 3
};

// Object: Enum Feature_Arena.EArenaLoadPriority
enum class EArenaLoadPriority : uint8_t
{
	High_Priority = 0,
	Medium_Priority = 1,
	Low_Priority = 2,
	EArenaLoadPriority_MAX = 3
};

// Object: Enum Feature_Arena.EArenaPropState
enum class EArenaPropState : uint8_t
{
	NotActivate = 0,
	NotPick = 1,
	Picked = 2,
	PendingKill = 3,
	EArenaPropState_MAX = 4
};

// Object: Enum Feature_Arena.EArenaPropValType
enum class EArenaPropValType : uint8_t
{
	APVT_UNKNOWN = 0,
	APVT_INT32 = 1,
	APVT_UINT32 = 2,
	APVT_INT64 = 3,
	APVT_UINT64 = 4,
	APVT_FLOAT = 5,
	APVT_DOUBLE = 6,
	APVT_BOOLEAN = 7,
	APVT_STRING = 8,
	APVT_VECTOR2 = 9,
	APVT_VECTOR = 10,
	APVT_VECTOR4 = 11,
	APVT_ROTATOR = 12,
	APVT_LUATABLE = 13,
	MAX = 127
};

// Object: Enum Feature_Arena.EUpdateInfoType
enum class EUpdateInfoType : uint8_t
{
	Damage = 0,
	Heal = 1,
	Guard = 2,
	EUpdateInfoType_MAX = 3
};

// Object: Enum Feature_Arena.EPerfectTimeClass
enum class EPerfectTimeClass : uint8_t
{
	InValid = 0,
	Good = 1,
	Cool = 2,
	Perfect = 3,
	EPerfectTimeClass_MAX = 4
};

// Object: Enum Feature_Arena.EPerfectTimeTriggerReason
enum class EPerfectTimeTriggerReason : uint8_t
{
	InValid = 0,
	Interrupt = 1,
	TaskEnd = 2,
	SkillTrigger = 3,
	EPerfectTimeTriggerReason_MAX = 4
};

// Object: Enum Feature_Arena.EArenaSkillCDType
enum class EArenaSkillCDType : uint8_t
{
	NormalSkillCd = 0,
	ChargeSkillCd = 1,
	TimeChargeSkillCd = 2,
	EArenaSkillCDType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaClearSkillChangeStateReason
enum class EArenaClearSkillChangeStateReason : uint8_t
{
	Invalid = 0,
	OwnerDead = 1,
	OwnerSkillUnEquip = 2,
	BuffEnd = 3,
	CastLastChangeSkill = 4,
	TimeOut = 5,
	SubGameEnd = 6,
	ChangeSkillTaskEnd = 7,
	FirstSkillReject = 8,
	ChangeNewSkill = 9,
	RSQ = 10,
	EArenaClearSkillChangeStateReason_MAX = 11
};

// Object: Enum Feature_Arena.EArenaSkillChangeStateReason
enum class EArenaSkillChangeStateReason : uint8_t
{
	Invalid = 0,
	Buff = 1,
	CastChangeSkill = 2,
	ChangeSkillTask = 3,
	RSQ = 4,
	EArenaSkillChangeStateReason_MAX = 5
};

// Object: Enum Feature_Arena.ECastSkillResultEnum
enum class ECastSkillResultEnum : uint8_t
{
	SkillCastSucceed = 1,
	InvalidSkillId = 2,
	SkillNotEquipped = 3,
	SkillInCd = 4,
	SkillChargeNotReady = 5,
	SkillNoTimeline = 6,
	SkillStateNotSatisfy = 7,
	SkillTargetLockButNoTarget = 8,
	SkillNoTargetWithSearchTarget = 9,
	SkillTargetNotInFilterRule = 10,
	SkillSlotForbidden = 11,
	MainSkillCastFailed = 12,
	WaitingServerCastSkillCheck = 13,
	SubSkillConfigCheckFailed = 14,
	DuplicatedSkillInstId = 15,
	RSQRejected = 16,
	SkillHasNoTimerChargeNum = 17,
	ColaNotReady = 18,
	ECastSkillResultEnum_MAX = 19
};

// Object: Enum Feature_Arena.EArenaSkillSlot
enum class EArenaSkillSlot : uint8_t
{
	ASS_Invalid = 0,
	ASS_NormalAttack = 1,
	ASS_SkillOne = 2,
	ASS_SkillTwo = 3,
	ASS_SkillUltimate = 4,
	ASS_SkillExtraOne = 5,
	ASS_SkillExtraTwo = 6,
	ASS_SkillReturnHome = 7,
	ASS_SkillSummoner = 8,
	ASS_SkillSpecialMode = 9,
	ASS_SkillPortal = 10,
	ASS_SkillSummonTotem = 11,
	ASS_SkillJump = 12,
	ASS_SkillKillHongSun = 13,
	ASS_SkillTeleport = 15,
	ASS_MAX = 16
};

// Object: Enum Feature_Arena.EArenaSkillCacheEndReason
enum class EArenaSkillCacheEndReason : uint8_t
{
	TimeOut = 1,
	SkillHasStart = 2,
	EArenaSkillCacheEndReason_MAX = 3
};

// Object: Enum Feature_Arena.EArenaSkillObjectType
enum class EArenaSkillObjectType : uint8_t
{
	Default = 1,
	Skill_Buff = 2,
	Skill_Effect = 3,
	Skill_Instance = 4,
	Buff_Instance = 5,
	EArenaSkillObjectType_MAX = 6
};

// Object: Enum Feature_Arena.EArenaPoolMethod
enum class EArenaPoolMethod : uint8_t
{
	None = 0,
	ManualRelease = 1,
	FreeInPool = 2,
	EArenaPoolMethod_MAX = 3
};

// Object: Enum Feature_Arena.EArenaVerifyType
enum class EArenaVerifyType : uint8_t
{
	MoveSpeed = 0,
	FallingHeight = 1,
	FallingTimes = 2,
	VerifyType = 3,
	VerifyType = 4,
	VerifyType = 5,
	VerifyType = 6,
	VerifyType = 7,
	VerifyType = 8,
	VerifyType = 9,
	EArenaVerifyType_MAX = 10
};

// Object: Enum Feature_Arena.EArenaSMBoxCheckMask
enum class EArenaSMBoxCheckMask : uint8_t
{
	OpenCheckMask = 1,
	MoveMask = 2,
	FallingMask = 4,
	OnlyServerMask = 8,
	CheckMask = 16,
	CheckMask = 32,
	CheckMask = 64,
	CheckMask = 128,
	EArenaSMBoxCheckMask_MAX = 129
};

// Object: Enum Feature_Arena.EArenaForce3pType
enum class EArenaForce3pType : uint8_t
{
	Default = 0,
	Force3P = 1,
	Force1P = 2,
	EArenaForce3pType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaAirWallCollisionType
enum class EArenaAirWallCollisionType : uint8_t
{
	NoCollision = 0,
	BlockCharacter = 1,
	BlockAll = 2,
	EArenaAirWallCollisionType_MAX = 3
};

// Object: Enum Feature_Arena.EArenaSummonAIState
enum class EArenaSummonAIState : uint8_t
{
	Invalid = 0,
	NotActivate = 2,
	ReservedOne = 4,
	SummonUnite = 8,
	ReservedTwo = 16,
	SelfDriven = 32,
	ReservedThree = 64,
	EArenaSummonAIState_MAX = 65
};

// Object: Enum Feature_Arena.EArenaSummonNotActivateReason
enum class EArenaSummonNotActivateReason : uint8_t
{
	None = 0,
	NotInRound = 1,
	IsDormant = 2,
	IsRespawning = 4,
	EArenaSummonNotActivateReason_MAX = 5
};

// Object: Enum Feature_Arena.EArenaCarRunningState
enum class EArenaCarRunningState : uint8_t
{
	None = 0,
	Loop = 1,
	NoDriver = 2,
	End = 3,
	EArenaCarRunningState_MAX = 4
};

// Object: Enum Feature_Arena.EArenaCombatEventCommonTargetType
enum class EArenaCombatEventCommonTargetType : uint8_t
{
	ARENA_TargetType_Source = 0,
	ARENA_TargetType_Target = 1,
	ARENA_TargetType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaLastHitDataType
enum class EArenaLastHitDataType : uint8_t
{
	LastHit = 0,
	LastBeHit = 1,
	EArenaLastHitDataType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaLastHitConsumeType
enum class EArenaLastHitConsumeType : uint8_t
{
	WithoutConsume = 0,
	ConsumeAll = 1,
	ConsumeType = 2,
	ConsumeByTime = 3,
	EArenaLastHitConsumeType_MAX = 4
};

// Object: Enum Feature_Arena.EArenaTimelineAbleType
enum class EArenaTimelineAbleType : uint8_t
{
	Default = 0,
	Skill = 1,
	Buff = 2,
	EArenaTimelineAbleType_MAX = 3
};

// Object: Enum Feature_Arena.EShowTipItemType
enum class EShowTipItemType : uint8_t
{
	Damage = 0,
	CriticalDamage = 1,
	Heal = 2,
	Other = 3,
	EShowTipItemType_MAX = 4
};

// Object: Enum Feature_Arena.EDamagePosTipType
enum class EDamagePosTipType : uint8_t
{
	Normal = 0,
	BigNumber = 1,
	NormalCritical = 2,
	BigNumberCritical = 3,
	BuffNormal = 4,
	BuffBigNumber = 5,
	BuffCritical = 6,
	BuffBigNumberCritical = 7,
	EDamagePosTipType_MAX = 8
};

// Object: Enum Feature_Arena.EArenaFBGameState
enum class EArenaFBGameState : uint8_t
{
	Preparing = 1,
	Combating = 2,
	EArenaFBGameState_MAX = 3
};

// Object: Enum Feature_Arena.EMiniMapItem
enum class EMiniMapItem : uint8_t
{
	Item_None = 0,
	Item_Tower = 1,
	Item_Crystal = 2,
	Item_Prop = 3,
	Item_Monster = 4,
	Item_Mount = 5,
	Item_Player = 6,
	Item_Bullet = 7,
	Item_Dominate = 8,
	Item_Tyrant = 9,
	Item_FlashLoong = 10,
	Item_DarkLoong = 11,
	Item_CapturePoint = 12,
	Item_BuffIcon = 13,
	Item_Mushroom = 14,
	Item_MAX = 15
};

// Object: Enum Feature_Arena.EHOKUpperElementShowFlag
enum class EHOKUpperElementShowFlag : uint8_t
{
	ShowEye = 1,
	GongSunLiEnemyStack = 2,
	EHOKUpperElementShowFlag_MAX = 3
};

// Object: Enum Feature_Arena.EHOKMonsterMaterialParamSlot
enum class EHOKMonsterMaterialParamSlot : uint8_t
{
	SideColor = 0,
	HitRed = 1,
	Frozen = 2,
	Transparent = 3,
	RedScar0 = 4,
	RedScar1 = 5,
	RedScar2 = 6,
	RedScar3 = 7,
	WhiteOutline = 8,
	Max = 9
};

// Object: Enum Feature_Arena.EHOKMonsterAbnormalState
enum class EHOKMonsterAbnormalState : uint8_t
{
	HOKMAS_Frozen = 0,
	Max = 1
};

// Object: Enum Feature_Arena.EHOKHeroPosition
enum class EHOKHeroPosition : uint8_t
{
	HOK_HT_None = 0,
	HOK_HT_Mid = 1,
	HOK_HT_Top = 2,
	HOK_HT_ADC = 3,
	HOK_HT_Jungle = 4,
	HOK_HT_Support = 5,
	HOK_HT_MAX = 6
};

// Object: Enum Feature_Arena.EBlackHoleKeyType
enum class EBlackHoleKeyType : uint8_t
{
	Normal = 0,
	Tyrant = 1,
	Dominate = 2,
	EBlackHoleKeyType_MAX = 3
};

// Object: Enum Feature_Arena.EHOKMapAreaType
enum class EHOKMapAreaType : uint8_t
{
	Normal = 0,
	Highlands = 1,
	RedBuffArea = 2,
	BlueBuffArea = 3,
	NashorRoad = 4,
	DragonRoad = 5,
	SingleRoad = 6,
	MidRoad = 7,
	DoubleRoad = 8,
	StoneArea = 9,
	InsectsArea = 10,
	BigSmallDragonViewArea = 11,
	JungleViewArea = 12,
	HomeViewArea = 13,
	JungleDamageChangeArea = 14,
	EHOKMapAreaType_MAX = 15
};

// Object: Enum Feature_Arena.EHOKMonsterDeadState
enum class EHOKMonsterDeadState : uint8_t
{
	HMDS_Alive = 0,
	HMDS_NormalDead = 1,
	HMDS_RunAwayDead = 2,
	HMDS_MAX = 3
};

// Object: Enum Feature_Arena.EHOKMountStateEnum
enum class EHOKMountStateEnum : uint8_t
{
	HOK_None = 0,
	HOK_CD = 1,
	HOK_BornInHome = 2,
	HOK_BeUsing = 3,
	HOK_WaitingByThrow = 4,
	HOK_CDByThrow = 5,
	HOK_Die = 6,
	HOK_MAX = 7
};

// Object: Enum Feature_Arena.EPathFindingStatus
enum class EPathFindingStatus : uint8_t
{
	Pending = 0,
	Processing = 1,
	Completed = 2,
	Failed = 3,
	Cancelled = 4,
	EPathFindingStatus_MAX = 5
};

// Object: Enum Feature_Arena.EPathFindingPriority
enum class EPathFindingPriority : uint8_t
{
	Low = 0,
	Normal = 1,
	High = 2,
	Critical = 3,
	EPathFindingPriority_MAX = 4
};

// Object: Enum Feature_Arena.ELockEnemyUpdateType
enum class ELockEnemyUpdateType : uint8_t
{
	Press = 0,
	Drag = 1,
	ELockEnemyUpdateType_MAX = 2
};

// Object: Enum Feature_Arena.EArenaNameBarRightElement_CommonElementPriority
enum class EArenaNameBarRightElement_CommonElementPriority : uint8_t
{
	invalid = 0,
	w_overlay_IconSkill = 1,
	BuffElement = 2,
	EArenaNameBarRightElement_MAX = 3
};

// Object: Enum Feature_Arena.EUpElementUIType
enum class EUpElementUIType : uint8_t
{
	EUpElementUIType_Stack = 0,
	EUpElementUIType_NoStack = 1,
	EUpElementUIType_MAX = 2
};

// Object: Enum Feature_Arena.EUpdateUIReason
enum class EUpdateUIReason : uint8_t
{
	EUpdateUIReason_Invalid = 0,
	EUpdateUIReason_Add = 1,
	EUpdateUIReason_Remove = 2,
	EUpdateUIReason_Update = 3,
	EUpdateUIReason_MAX = 4
};

// Object: Enum Feature_Arena.EArenaNinjaMarkAnimType
enum class EArenaNinjaMarkAnimType : uint8_t
{
	NotPlayAnimation = 0,
	PlayIntermediateAnim = 1,
	PlayCompletedAnim = 2,
	PlayFlickerAnim = 3,
	ForeRefreshAnim = 4,
	EArenaNinjaMarkAnimType_MAX = 5
};

// Object: Enum Feature_Arena.ECDShowType
enum class ECDShowType : uint8_t
{
	ECDShowType_Normal = 0,
	ECDShowType_Charge = 1,
	ECDShowType_MAX = 2
};

// Package: Feature_Arena_Core
// Enums: 6

// Object: Enum Feature_Arena_Core.EArenaRoleSelectPhase
enum class EArenaRoleSelectPhase : uint16_t
{
	None = 0,
	AvatarShow = 1,
	BanHero = 2,
	SelectHero = 3,
	SelectSkin = 4,
	Over = 255,
	EArenaRoleSelectPhase_MAX = 256
};

// Object: Enum Feature_Arena_Core.EArenaGameDataSecFlow
enum class EArenaGameDataSecFlow : uint8_t
{
	EArenaGameDataSecFlow_Invalid = 0,
	EArenaGameDataSecFlow_Start = 1,
	EArenaGameDataSecFlow_End = 2,
	EArenaGameDataSecFlow_MAX = 3
};

// Object: Enum Feature_Arena_Core.ESubGuideFlowType
enum class ESubGuideFlowType : uint8_t
{
	VeteranSkip = 0,
	HeroSelectNew = 1,
	CardSelectNew = 2,
	ESubGuideFlowType_MAX = 3
};

// Object: Enum Feature_Arena_Core.EArenaSequenceType
enum class EArenaSequenceType : uint8_t
{
	None = 0,
	Prepare = 1,
	CardShow = 2,
	Final = 3,
	EArenaSequenceType_MAX = 4
};

// Object: Enum Feature_Arena_Core.EArenaPlayerTeamActiveState
enum class EArenaPlayerTeamActiveState : uint8_t
{
	Active = 0,
	Eliminated = 1,
	Victory = 2,
	Bye = 3,
	EArenaPlayerTeamActiveState_MAX = 4
};

// Object: Enum Feature_Arena_Core.EArenaGameEliminatedReason
enum class EArenaGameEliminatedReason : uint8_t
{
	Normal = 0,
	ActiveQuit = 1,
	EArenaGameEliminatedReason_MAX = 2
};

// Package: CrowdSystem
// Enums: 1

// Object: Enum CrowdSystem.ECrowdV2BlendWeightSource
enum class ECrowdV2BlendWeightSource : uint8_t
{
	Custom = 0,
	CharacterSpeed = 1,
	CharacterSpeedSmooth = 2,
	ECrowdV2BlendWeightSource_MAX = 3
};

// Package: Feature_Arena_HOK
// Enums: 33

// Object: Enum Feature_Arena_HOK.EArenaMovementMode
enum class EArenaMovementMode : uint8_t
{
	ARENA_MOVE_None = 0,
	ARENA_MOVE_NavWalking = 1,
	ARENA_MOVE_Die = 2,
	ARENA_MOVE_TaskMovement = 3,
	ARENA_MOVE_Max = 4
};

// Object: Enum Feature_Arena_HOK.ELineMonsterStageType
enum class ELineMonsterStageType : uint8_t
{
	LMST_Normal = 0,
	LMST_ReturningToInit = 1,
	LMST_ExcitedDueToFriendAttacked = 2,
	LMST_MAX = 3
};

// Object: Enum Feature_Arena_HOK.EHOKRangeShape
enum class EHOKRangeShape : uint8_t
{
	HOK_Rect = 0,
	HOK_Circle = 1,
	HOK_Sector = 2,
	HOK_MAX = 3
};

// Object: Enum Feature_Arena_HOK.EHOKGetActorPriorityEnum
enum class EHOKGetActorPriorityEnum : uint8_t
{
	HOK_Dis = 0,
	HOK_HP = 1,
	HOK_HPPercent = 2,
	HOK_MAX = 3
};

// Object: Enum Feature_Arena_HOK.EHOKActorType
enum class EHOKActorType : uint8_t
{
	HOK_Tower = 0,
	HOK_Monster = 1,
	HOK_Character = 2,
	HOK_Crystal = 3,
	HOK_Prop = 4,
	HOK_Solider = 5,
	HOK_Spring = 6,
	HOK_Mount = 7,
	HOK_OccupiedCircle = 8,
	HOK_ViewItem = 9,
	HOK_SummonChar = 10,
	HOK_Num = 11,
	HOK_MAX = 12
};

// Object: Enum Feature_Arena_HOK.EHOKArtifactWeatherEnum
enum class EHOKArtifactWeatherEnum : uint8_t
{
	None = 0,
	Frozen = 1,
	Whirlwind = 2,
	AcidRain = 3,
	Lightning = 4,
	Meteor = 5,
	EHOKArtifactWeatherEnum_MAX = 6
};

// Object: Enum Feature_Arena_HOK.EHOKArtifactQualityEnum
enum class EHOKArtifactQualityEnum : uint8_t
{
	Normal = 0,
	Golden = 1,
	Colorful = 2,
	Dazzling = 3,
	EHOKArtifactQualityEnum_MAX = 4
};

// Object: Enum Feature_Arena_HOK.EHOKCullingUnitState
enum class EHOKCullingUnitState : uint8_t
{
	UnitState_Default = 0,
	UnitState_Inside = 2,
	UnitState_Outside = 3,
	UnitState_MAX = 4
};

// Object: Enum Feature_Arena_HOK.EHOKCullingCameraState
enum class EHOKCullingCameraState : uint8_t
{
	CameraState_Default = 0,
	CameraState_Minimap = 1,
	CameraState_MAX = 2
};

// Object: Enum Feature_Arena_HOK.EHOKFogCamp
enum class EHOKFogCamp : uint8_t
{
	Blue = 0,
	Red = 1,
	Neutral = 99,
	EHOKFogCamp_MAX = 100
};

// Object: Enum Feature_Arena_HOK.EHOKStagePhase
enum class EHOKStagePhase : uint8_t
{
	Prepare = 0,
	Battle = 1,
	Finish = 2,
	EHOKStagePhase_MAX = 3
};

// Object: Enum Feature_Arena_HOK.EHOKHideHeadIconReason
enum class EHOKHideHeadIconReason : uint8_t
{
	HHHIR_Setting = 0,
	HHHIR_Dead = 1,
	HHHIR_MAX = 2
};

// Object: Enum Feature_Arena_HOK.FHOKManagerEnum
enum class EFHOKManagerEnum : uint8_t
{
	ObjectPool = 0,
	MiniMapCanvasDrawMgr = 1,
	BlackHoleMgr = 2,
	StaticVariableMgr = 3,
	HOKHPBarMgr = 4,
	HOKActorMgr = 5,
	HOKMonsterMgr = 6,
	HOKHeroHeadIconMgr = 7,
	HOKUIMgr = 8,
	Max = 9
};

// Object: Enum Feature_Arena_HOK.EDrawFrameType
enum class EDrawFrameType : uint8_t
{
	DrawItemLow = 0,
	DrawItemHigh = 1,
	TowerUpdate = 2,
	CrystalUpdate = 3,
	PropUpdate = 4,
	MonsterUpdate = 5,
	LoongUpdate = 6,
	MountUpdate = 7,
	PlayerUpdate = 8,
	BulletUpdate = 9,
	OccupiedUpdate = 10,
	BuffIconUpdate = 11,
	MushroomUpdate = 12,
	EDrawFrameType_MAX = 13
};

// Object: Enum Feature_Arena_HOK.EHOKMapGroupShowState
enum class EHOKMapGroupShowState : uint8_t
{
	Alive = 0,
	Dead = 1,
	CD = 2,
	EHOKMapGroupShowState_MAX = 3
};

// Object: Enum Feature_Arena_HOK.EHOKMonsterType
enum class EHOKMonsterType : uint8_t
{
	Other = 0,
	Ordinary = 1,
	Elite = 2,
	EHOKMonsterType_MAX = 3
};

// Object: Enum Feature_Arena_HOK.EJungleMonsterRefreshTargetStage
enum class EJungleMonsterRefreshTargetStage : uint8_t
{
	Sleeping = 0,
	Aggressive = 1,
	Impatient = 2,
	EJungleMonsterRefreshTargetStage_MAX = 3
};

// Object: Enum Feature_Arena_HOK.LineMonsterTargetType
enum class ELineMonsterTargetType : uint8_t
{
	LMTT_None = 0,
	LMTT_Character = 1,
	LMTT_TowerAndCrystal = 2,
	LMTT_Monster = 3,
	LMTT_MAX = 4
};

// Object: Enum Feature_Arena_HOK.EClientSeeState
enum class EClientSeeState : uint8_t
{
	NotSee = 0,
	HalfTransparent = 1,
	TotalSee = 2,
	EClientSeeState_MAX = 3
};

// Object: Enum Feature_Arena_HOK.ECrowdV2MonsterState
enum class ECrowdV2MonsterState : uint8_t
{
	Idle = 0,
	Move = 1,
	Dead = 2,
	RunAwayDead = 3,
	Timeline0 = 4,
	Timeline1 = 5,
	None = 6,
	ECrowdV2MonsterState_MAX = 7
};

// Object: Enum Feature_Arena_HOK.EHOKMonsterCrowdOverlayMaterialTag
enum class EHOKMonsterCrowdOverlayMaterialTag : uint8_t
{
	None = 0,
	FrozenBody = 1,
	EHOKMonsterCrowdOverlayMaterialTag_MAX = 2
};

// Object: Enum Feature_Arena_HOK.ETransparentSetReason
enum class ETransparentSetReason : uint8_t
{
	ETSR_BaseViewManager = 0,
	ETSR_ClientView = 1,
	ETSR_MAX = 2
};

// Object: Enum Feature_Arena_HOK.EOccupiedPlace
enum class EOccupiedPlace : uint8_t
{
	None = 0,
	Light = 1,
	Dark = 2,
	Top = 3,
	Mid = 4,
	Bottom = 5,
	Jungle = 6,
	EOccupiedPlace_MAX = 7
};

// Object: Enum Feature_Arena_HOK.EOccupiedRule
enum class EOccupiedRule : uint8_t
{
	Unlimited = 0,
	OnePerType = 1,
	OneTotal = 2,
	EOccupiedRule_MAX = 3
};

// Object: Enum Feature_Arena_HOK.EOccupiedType
enum class EOccupiedType : uint8_t
{
	None = 0,
	LightAndDarkDragonKing = 1,
	PleasantGoat = 2,
	EOccupiedType_MAX = 3
};

// Object: Enum Feature_Arena_HOK.EOccupiedState
enum class EOccupiedState : uint8_t
{
	Idle = 0,
	Increasing = 1,
	Decreasing = 2,
	Paused = 3,
	Completed = 4,
	EOccupiedState_MAX = 5
};

// Object: Enum Feature_Arena_HOK.EHOKMonsterKillByErrorReason
enum class EHOKMonsterKillByErrorReason : uint8_t
{
	HMKBER_None = 0,
	HMKBER_NavMeshNotFound = 1,
	HMKBER_LocationTooLow = 2,
	HMKBER_MAX = 3
};

// Object: Enum Feature_Arena_HOK.EHOKGemState
enum class EHOKGemState : uint8_t
{
	InActive = 0,
	Idle = 1,
	Dropping = 2,
	FlashDying = 3,
	AbsorbToPlayer = 4,
	EHOKGemState_MAX = 5
};

// Object: Enum Feature_Arena_HOK.FruitState
enum class EFruitState : uint8_t
{
	Small = 0,
	Big = 1,
	Flowering = 2,
	FruitState_MAX = 3
};

// Object: Enum Feature_Arena_HOK.EHOKSupportClothState
enum class EHOKSupportClothState : uint8_t
{
	HTS_None = 0,
	HTS_TakeOnProgressBar = 1,
	HTS_TakeOnCloth_NoOut = 2,
	HTS_TakeOnCloth_Out = 3,
	HTS_TakeOffProgressBar = 4,
	HTS_TakeOffCloth_NoOut = 5,
	HTS_TakeOffCloth_Out = 6,
	HTS_Trigger_CD = 7,
	HTS_MAX = 8
};

// Object: Enum Feature_Arena_HOK.EHOKTeleportState
enum class EHOKTeleportState : uint8_t
{
	HTS_Wait = 0,
	HTS_Open = 1,
	HTS_Close = 2,
	HTS_End = 3,
	HTS_MAX = 4
};

// Object: Enum Feature_Arena_HOK.EHOKUIType
enum class EHOKUIType : uint8_t
{
	UI_HOK_AttackBtn = 0,
	UI_HOK_InLevelPrepare_TopDetail = 1,
	UI_HOK_BuffItem = 2,
	UI_HOK_Blood_MonsterBig = 3,
	UI_HOK_Blood_MonsterMid = 4,
	UI_HOK_Blood_MonsterSmall = 5,
	UI_HOK_Blood_Soldier = 6,
	UI_HOK_NameBar_Up_Element = 7,
	UI_Arena_ShowEye = 8,
	UI_HOK_HeroUpTips = 9,
	UI_HOK_Billboard_MonsterBase = 10,
	UI_HOK_Blood_MonsterBase = 11,
	UI_MAX = 12
};

// Object: Enum Feature_Arena_HOK.EHOKWidgetTweenType
enum class EHOKWidgetTweenType : uint8_t
{
	Linear = 0,
	Oval = 1,
	EHOKWidgetTweenType_MAX = 2
};

// Package: Feature_DND
// Enums: 2

// Object: Enum Feature_DND.EDNDGlobalCheatFlag
enum class EDNDGlobalCheatFlag : uint8_t
{
	DDBCheatFlag_EnableChasingBallDrawDebug = 0,
	DDBCheatFlag_MAX = 1
};

// Object: Enum Feature_DND.EDNDGameEliminatedReason
enum class EDNDGameEliminatedReason : uint8_t
{
	Normal = 0,
	ActiveQuit = 1,
	EDNDGameEliminatedReason_MAX = 2
};

// Package: ACMOcean
// Enums: 1

// Object: Enum ACMOcean.EInteractState
enum class EInteractState : uint8_t
{
	None = 0,
	Idling = 1,
	Running = 2,
	Swimming = 3,
	Anchoring = 4,
	Sailing = 5,
	EInteractState_MAX = 6
};

// Package: PhysicsCore
// Enums: 10

// Object: Enum PhysicsCore.EBodyCollisionResponse
enum class EBodyCollisionResponse : uint8_t
{
	BodyCollision_Enabled = 0,
	BodyCollision_Disabled = 1,
	BodyCollision_MAX = 2
};

// Object: Enum PhysicsCore.EPhysicsType
enum class EPhysicsType : uint8_t
{
	PhysType_Default = 0,
	PhysType_Kinematic = 1,
	PhysType_Simulated = 2,
	PhysType_MAX = 3
};

// Object: Enum PhysicsCore.ECollisionTraceFlag
enum class ECollisionTraceFlag : uint8_t
{
	CTF_UseDefault = 0,
	CTF_UseSimpleAndComplex = 1,
	CTF_UseSimpleAsComplex = 2,
	CTF_UseComplexAsSimple = 3,
	CTF_MAX = 4
};

// Object: Enum PhysicsCore.ELinearConstraintMotion
enum class ELinearConstraintMotion : uint8_t
{
	LCM_Free = 0,
	LCM_Limited = 1,
	LCM_Locked = 2,
	LCM_MAX = 3
};

// Object: Enum PhysicsCore.EConstraintFrame
enum class EConstraintFrame : uint8_t
{
	Frame1 = 0,
	Frame2 = 1,
	EConstraintFrame_MAX = 2
};

// Object: Enum PhysicsCore.EAngularConstraintMotion
enum class EAngularConstraintMotion : uint8_t
{
	ACM_Free = 0,
	ACM_Limited = 1,
	ACM_Locked = 2,
	ACM_MAX = 3
};

// Object: Enum PhysicsCore.ESleepFamily
enum class ESleepFamily : uint8_t
{
	Normal = 0,
	Sensitive = 1,
	Custom = 2,
	ESleepFamily_MAX = 3
};

// Object: Enum PhysicsCore.ERadialImpulseFalloff
enum class ERadialImpulseFalloff : uint8_t
{
	RIF_Constant = 0,
	RIF_Linear = 1,
	RIF_MAX = 2
};

// Object: Enum PhysicsCore.EPhysicalSurface
enum class EPhysicalSurface : uint8_t
{
	SurfaceType_Default = 0,
	SurfaceType1 = 1,
	SurfaceType2 = 2,
	SurfaceType3 = 3,
	SurfaceType4 = 4,
	SurfaceType5 = 5,
	SurfaceType6 = 6,
	SurfaceType7 = 7,
	SurfaceType8 = 8,
	SurfaceType9 = 9,
	SurfaceType10 = 10,
	SurfaceType11 = 11,
	SurfaceType12 = 12,
	SurfaceType13 = 13,
	SurfaceType14 = 14,
	SurfaceType15 = 15,
	SurfaceType16 = 16,
	SurfaceType17 = 17,
	SurfaceType18 = 18,
	SurfaceType19 = 19,
	SurfaceType20 = 20,
	SurfaceType21 = 21,
	SurfaceType22 = 22,
	SurfaceType23 = 23,
	SurfaceType24 = 24,
	SurfaceType25 = 25,
	SurfaceType26 = 26,
	SurfaceType27 = 27,
	SurfaceType28 = 28,
	SurfaceType29 = 29,
	SurfaceType30 = 30,
	SurfaceType31 = 31,
	SurfaceType32 = 32,
	SurfaceType33 = 33,
	SurfaceType34 = 34,
	SurfaceType35 = 35,
	SurfaceType36 = 36,
	SurfaceType37 = 37,
	SurfaceType38 = 38,
	SurfaceType39 = 39,
	SurfaceType40 = 40,
	SurfaceType41 = 41,
	SurfaceType42 = 42,
	SurfaceType43 = 43,
	SurfaceType44 = 44,
	SurfaceType45 = 45,
	SurfaceType46 = 46,
	SurfaceType47 = 47,
	SurfaceType48 = 48,
	SurfaceType49 = 49,
	SurfaceType50 = 50,
	SurfaceType51 = 51,
	SurfaceType52 = 52,
	SurfaceType53 = 53,
	SurfaceType54 = 54,
	SurfaceType55 = 55,
	SurfaceType56 = 56,
	SurfaceType57 = 57,
	SurfaceType58 = 58,
	SurfaceType59 = 59,
	SurfaceType60 = 60,
	SurfaceType61 = 61,
	SurfaceType62 = 62,
	SurfaceType_Max = 63,
	EPhysicalSurface_MAX = 64
};

// Object: Enum PhysicsCore.EFrictionCombineMode
enum class EFrictionCombineMode : uint8_t
{
	Average = 0,
	Min = 1,
	Multiply = 2,
	Max = 3
};

// Package: SPInputSystem
// Enums: 9

// Object: Enum SPInputSystem.ESPInputPlatform
enum class ESPInputPlatform : uint8_t
{
	None = 0,
	PC = 1,
	Mobile = 2,
	Console = 3,
	ESPInputPlatform_MAX = 4
};

// Object: Enum SPInputSystem.ESPInputType
enum class ESPInputType : uint8_t
{
	None = 0,
	Digital = 1,
	Axis = 2,
	ESPInputType_MAX = 3
};

// Object: Enum SPInputSystem.ESPInputTagRemovedReason
enum class ESPInputTagRemovedReason : uint8_t
{
	None = 0,
	ConsumedNormal = 1,
	ConsumedFromCache = 2,
	ActiveToNone = 3,
	CacheToNone = 4,
	OrphanedNoContext = 5,
	Reset = 6,
	ActiveToOnGoing = 7,
	CachedToOnGoing = 8,
	ManualClear = 9,
	ESPInputTagRemovedReason_MAX = 10
};

// Object: Enum SPInputSystem.ESPInputResetType
enum class ESPInputResetType : uint8_t
{
	CachedTriggerOnly = 0,
	AllRelatedTriggers = 1,
	Full = 2,
	ESPInputResetType_MAX = 3
};

// Object: Enum SPInputSystem.ESPInputErrorCode
enum class ESPInputErrorCode : uint8_t
{
	None = 0,
	NullPointer = 1,
	SubsystemNotFound = 2,
	ContextNotFound = 3,
	InvalidContextID = 4,
	TriggerRegistrationFailed = 100,
	TriggerRemovalFailed = 101,
	ControlComponentNotFound = 102,
	ActionGrantFailed = 200,
	ActionRevokeFailed = 201,
	ActionComponentNotFound = 202,
	ESPInputErrorCode_MAX = 203
};

// Object: Enum SPInputSystem.ESPActionExecutionResult
enum class ESPActionExecutionResult : uint8_t
{
	Success = 0,
	CannotExecute = 1,
	NotFound = 2,
	ESPActionExecutionResult_MAX = 3
};

// Object: Enum SPInputSystem.ETriggerEvalResult
enum class ETriggerEvalResult : uint8_t
{
	None = 0,
	Ongoing = 1,
	Triggered = 2,
	ETriggerEvalResult_MAX = 3
};

// Object: Enum SPInputSystem.ESPInputTriggerState
enum class ESPInputTriggerState : uint8_t
{
	None = 0,
	OnGoing = 1,
	Active = 2,
	Consumed = 3,
	Cached = 4,
	ESPInputTriggerState_MAX = 5
};

// Object: Enum SPInputSystem.ESPInputKeyState
enum class ESPInputKeyState : uint8_t
{
	Up = 0,
	Down = 1,
	ESPInputKeyState_MAX = 2
};

// Package: LetsGoCommonUtils
// Enums: 2

// Object: Enum LetsGoCommonUtils.ELetsGoExcelColor
enum class ELetsGoExcelColor : uint8_t
{
	None = 0,
	Black = 1,
	White = 2,
	Red = 3,
	Green = 4,
	Blue = 5,
	Yellow = 6,
	ELetsGoExcelColor_MAX = 7
};

// Object: Enum LetsGoCommonUtils.ELetsGoExcelCellType
enum class ELetsGoExcelCellType : uint8_t
{
	CELLTYPE_EMPTY = 0,
	CELLTYPE_NUMBER = 1,
	CELLTYPE_STRING = 2,
	CELLTYPE_BOOLEAN = 3,
	CELLTYPE_BLANK = 4,
	CELLTYPE_ERROR = 5,
	CELLTYPE_MAX = 6
};

// Package: SPTimeOfDay
// Enums: 3

// Object: Enum SPTimeOfDay.ESPCurveTimeType
enum class ESPCurveTimeType : uint8_t
{
	SunZ = 0,
	TimeOfDay = 1,
	InternalTimeOfDay = 2,
	ESPCurveTimeType_MAX = 3
};

// Object: Enum SPTimeOfDay.ESPSkyMode
enum class ESPSkyMode : uint8_t
{
	E2DDynamicClouds = 0,
	E2DCloudsUsingColorCurve = 1,
	EVolumetricClouds = 2,
	ESPSkyMode_MAX = 3
};

// Object: Enum SPTimeOfDay.E__SPUDS_SkyMode__pf
enum class E__SPUDS_SkyMode__pf : uint8_t
{
	EVolumetricClouds = 0,
	EStaticClouds = 1,
	E2DDynamicClouds = 2,
	ENoClouds = 3,
	E2DCloudsUsingColorCurve = 4,
	EVolumetricAurora = 5,
	ENoCloudsUsingColorCurve = 6,
	E__SPUDS_SkyMode__MAX = 7
};

// Package: OodleNetworkHandlerComponent
// Enums: 1

// Object: Enum OodleNetworkHandlerComponent.EOodleNetworkEnableMode
enum class EOodleNetworkEnableMode : uint8_t
{
	OodleAlwaysEnabled = 0,
	OodleWhenCompressedPacketReceived = 1,
	EOodleNetworkEnableMode_MAX = 2
};

// Package: VariantManagerContent
// Enums: 1

// Object: Enum VariantManagerContent.EPropertyValueCategory
enum class EPropertyValueCategory : uint8_t
{
	Undefined = 0,
	Generic = 1,
	RelativeLocation = 2,
	RelativeRotation = 4,
	RelativeScale3D = 8,
	Visibility = 16,
	Material = 32,
	Color = 64,
	Option = 128,
	EPropertyValueCategory_MAX = 129
};

// Package: AudioMixer
// Enums: 4

// Object: Enum AudioMixer.EMusicalNoteName
enum class EMusicalNoteName : uint8_t
{
	C = 0,
	Db = 1,
	D = 2,
	Eb = 3,
	E = 4,
	F = 5,
	Gb = 6,
	G = 7,
	Ab = 8,
	A = 9,
	Bb = 10,
	B = 11,
	EMusicalNoteName_MAX = 12
};

// Object: Enum AudioMixer.ESubmixEffectDynamicsChannelLinkMode
enum class ESubmixEffectDynamicsChannelLinkMode : uint8_t
{
	Disabled = 0,
	Average = 1,
	Peak = 2,
	Count = 3,
	ESubmixEffectDynamicsChannelLinkMode_MAX = 4
};

// Object: Enum AudioMixer.ESubmixEffectDynamicsPeakMode
enum class ESubmixEffectDynamicsPeakMode : uint8_t
{
	MeanSquared = 0,
	RootMeanSquared = 1,
	Peak = 2,
	Count = 3,
	ESubmixEffectDynamicsPeakMode_MAX = 4
};

// Object: Enum AudioMixer.ESubmixEffectDynamicsProcessorType
enum class ESubmixEffectDynamicsProcessorType : uint8_t
{
	Compressor = 0,
	Limiter = 1,
	Expander = 2,
	Gate = 3,
	Count = 4,
	ESubmixEffectDynamicsProcessorType_MAX = 5
};

// Package: Water
// Enums: 4

// Object: Enum Water.EWaveSpectrumType
enum class EWaveSpectrumType : uint8_t
{
	Phillips = 0,
	PiersonMoskowitz = 1,
	JONSWAP = 2,
	EWaveSpectrumType_MAX = 3
};

// Object: Enum Water.EWaterBodyType
enum class EWaterBodyType : uint8_t
{
	River = 0,
	Lake = 1,
	Ocean = 2,
	Transition = 3,
	Num = 4,
	EWaterBodyType_MAX = 5
};

// Object: Enum Water.EWaterBrushBlendType
enum class EWaterBrushBlendType : uint8_t
{
	AlphaBlend = 0,
	Min = 1,
	Max = 2,
	Additive = 3
};

// Object: Enum Water.EWaterBrushFalloffMode
enum class EWaterBrushFalloffMode : uint8_t
{
	Angle = 0,
	Width = 1,
	EWaterBrushFalloffMode_MAX = 2
};

// Package: MediaAssets
// Enums: 8

// Object: Enum MediaAssets.EMediaWebcamCaptureDeviceFilter
enum class EMediaWebcamCaptureDeviceFilter : uint8_t
{
	None = 0,
	DepthSensor = 1,
	Front = 2,
	Rear = 4,
	Unknown = 8,
	EMediaWebcamCaptureDeviceFilter_MAX = 9
};

// Object: Enum MediaAssets.EMediaVideoCaptureDeviceFilter
enum class EMediaVideoCaptureDeviceFilter : uint8_t
{
	None = 0,
	Card = 1,
	Software = 2,
	Unknown = 4,
	Webcam = 8,
	EMediaVideoCaptureDeviceFilter_MAX = 9
};

// Object: Enum MediaAssets.EMediaAudioCaptureDeviceFilter
enum class EMediaAudioCaptureDeviceFilter : uint8_t
{
	None = 0,
	Card = 1,
	Microphone = 2,
	Software = 4,
	Unknown = 8,
	EMediaAudioCaptureDeviceFilter_MAX = 9
};

// Object: Enum MediaAssets.EMediaPlayerTrack
enum class EMediaPlayerTrack : uint8_t
{
	Audio = 0,
	Caption = 1,
	Metadata = 2,
	Script = 3,
	Subtitle = 4,
	Text = 5,
	Video = 6,
	EMediaPlayerTrack_MAX = 7
};

// Object: Enum MediaAssets.EMediaSoundComponentFFTSize
enum class EMediaSoundComponentFFTSize : uint8_t
{
	Min = 0,
	Small = 1,
	Medium = 2,
	Large = 3,
	EMediaSoundComponentFFTSize_MAX = 4
};

// Object: Enum MediaAssets.EMediaSoundChannels
enum class EMediaSoundChannels : uint8_t
{
	Mono = 0,
	Stereo = 1,
	Surround = 2,
	EMediaSoundChannels_MAX = 3
};

// Object: Enum MediaAssets.MediaTextureOrientation
enum class EMediaTextureOrientation : uint8_t
{
	MTORI_Original = 0,
	MTORI_CW90 = 1,
	MTORI_CW180 = 2,
	MTORI_CW270 = 3,
	MTORI_MAX = 4
};

// Object: Enum MediaAssets.MediaTextureOutputFormat
enum class EMediaTextureOutputFormat : uint8_t
{
	MTOF_Default = 0,
	MTOF_SRGB_LINOUT = 1,
	MTOF_MAX = 2
};

// Package: UdpMessaging
// Enums: 1

// Object: Enum UdpMessaging.EUdpMessageFormat
enum class EUdpMessageFormat : uint8_t
{
	None = 0,
	Json = 1,
	TaggedProperty = 2,
	CborPlatformEndianness = 3,
	CborStandardEndianness = 4,
	EUdpMessageFormat_MAX = 5
};

// Package: ActorSequence
// Enums: 1

// Object: Enum ActorSequence.EActorSequenceObjectReferenceType
enum class EActorSequenceObjectReferenceType : uint8_t
{
	ContextActor = 0,
	ExternalActor = 1,
	Component = 2,
	EActorSequenceObjectReferenceType_MAX = 3
};

// Package: OnlineSubsystem
// Enums: 2

// Object: Enum OnlineSubsystem.EInAppPurchaseState
enum class EInAppPurchaseState : uint8_t
{
	Unknown = 0,
	Success = 1,
	Failed = 2,
	Cancelled = 3,
	Invalid = 4,
	NotAllowed = 5,
	Restored = 6,
	AlreadyOwned = 7,
	EInAppPurchaseState_MAX = 8
};

// Object: Enum OnlineSubsystem.EMPMatchOutcome
enum class EMPMatchOutcome : uint8_t
{
	None = 0,
	Quit = 1,
	Won = 2,
	Lost = 3,
	Tied = 4,
	TimeExpired = 5,
	First = 6,
	Second = 7,
	Third = 8,
	Fourth = 9,
	EMPMatchOutcome_MAX = 10
};

// Package: AppleImageUtils
// Enums: 2

// Object: Enum AppleImageUtils.EAppleTextureType
enum class EAppleTextureType : uint8_t
{
	Unknown = 0,
	Image = 1,
	PixelBuffer = 2,
	Surface = 3,
	MetalTexture = 4,
	EAppleTextureType_MAX = 5
};

// Object: Enum AppleImageUtils.ETextureRotationDirection
enum class ETextureRotationDirection : uint8_t
{
	None = 0,
	Left = 1,
	Right = 2,
	Down = 3,
	LeftMirrored = 4,
	RightMirrored = 5,
	DownMirrored = 6,
	UpMirrored = 7,
	ETextureRotationDirection_MAX = 8
};

// Package: AssetTags
// Enums: 1

// Object: Enum AssetTags.ECollectionScriptingShareType
enum class ECollectionScriptingShareType : uint8_t
{
	Local = 0,
	Private = 1,
	Shared = 2,
	ECollectionScriptingShareType_MAX = 3
};

// Package: LocationServicesBPLibrary
// Enums: 1

// Object: Enum LocationServicesBPLibrary.ELocationAccuracy
enum class ELocationAccuracy : uint8_t
{
	LA_ThreeKilometers = 0,
	LA_OneKilometer = 1,
	LA_HundredMeters = 2,
	LA_TenMeters = 3,
	LA_Best = 4,
	LA_Navigation = 5,
	LA_MAX = 6
};

// Package: Synthesis
// Enums: 33

// Object: Enum Synthesis.ESynth1PatchDestination
enum class ESynth1PatchDestination : uint8_t
{
	Osc1Gain = 0,
	Osc1Frequency = 1,
	Osc1Pulsewidth = 2,
	Osc2Gain = 3,
	Osc2Frequency = 4,
	Osc2Pulsewidth = 5,
	FilterFrequency = 6,
	FilterQ = 7,
	Gain = 8,
	Pan = 9,
	LFO1Frequency = 10,
	LFO1Gain = 11,
	LFO2Frequency = 12,
	LFO2Gain = 13,
	Count = 14,
	ESynth1PatchDestination_MAX = 15
};

// Object: Enum Synthesis.ESynth1PatchSource
enum class ESynth1PatchSource : uint8_t
{
	LFO1 = 0,
	LFO2 = 1,
	Envelope = 2,
	BiasEnvelope = 3,
	Count = 4,
	ESynth1PatchSource_MAX = 5
};

// Object: Enum Synthesis.ESynthStereoDelayMode
enum class ESynthStereoDelayMode : uint8_t
{
	Normal = 0,
	Cross = 1,
	PingPong = 2,
	Count = 3,
	ESynthStereoDelayMode_MAX = 4
};

// Object: Enum Synthesis.ESynthFilterAlgorithm
enum class ESynthFilterAlgorithm : uint8_t
{
	OnePole = 0,
	StateVariable = 1,
	Ladder = 2,
	Count = 3,
	ESynthFilterAlgorithm_MAX = 4
};

// Object: Enum Synthesis.ESynthFilterType
enum class ESynthFilterType : uint8_t
{
	LowPass = 0,
	HighPass = 1,
	BandPass = 2,
	BandStop = 3,
	Count = 4,
	ESynthFilterType_MAX = 5
};

// Object: Enum Synthesis.ESynthModEnvBiasPatch
enum class ESynthModEnvBiasPatch : uint8_t
{
	PatchToNone = 0,
	PatchToOscFreq = 1,
	PatchToFilterFreq = 2,
	PatchToFilterQ = 3,
	PatchToLFO1Gain = 4,
	PatchToLFO2Gain = 5,
	PatchToLFO1Freq = 6,
	PatchToLFO2Freq = 7,
	Count = 8,
	ESynthModEnvBiasPatch_MAX = 9
};

// Object: Enum Synthesis.ESynthModEnvPatch
enum class ESynthModEnvPatch : uint8_t
{
	PatchToNone = 0,
	PatchToOscFreq = 1,
	PatchToFilterFreq = 2,
	PatchToFilterQ = 3,
	PatchToLFO1Gain = 4,
	PatchToLFO2Gain = 5,
	PatchToLFO1Freq = 6,
	PatchToLFO2Freq = 7,
	Count = 8,
	ESynthModEnvPatch_MAX = 9
};

// Object: Enum Synthesis.ESynthLFOPatchType
enum class ESynthLFOPatchType : uint8_t
{
	PatchToNone = 0,
	PatchToGain = 1,
	PatchToOscFreq = 2,
	PatchToFilterFreq = 3,
	PatchToFilterQ = 4,
	PatchToOscPulseWidth = 5,
	PatchToOscPan = 6,
	PatchLFO1ToLFO2Frequency = 7,
	PatchLFO1ToLFO2Gain = 8,
	Count = 9,
	ESynthLFOPatchType_MAX = 10
};

// Object: Enum Synthesis.ESynthLFOMode
enum class ESynthLFOMode : uint8_t
{
	Sync = 0,
	OneShot = 1,
	Free = 2,
	Count = 3,
	ESynthLFOMode_MAX = 4
};

// Object: Enum Synthesis.ESynthLFOType
enum class ESynthLFOType : uint8_t
{
	Sine = 0,
	UpSaw = 1,
	DownSaw = 2,
	Square = 3,
	Triangle = 4,
	Exponential = 5,
	RandomSampleHold = 6,
	Count = 7,
	ESynthLFOType_MAX = 8
};

// Object: Enum Synthesis.ESynth1OscType
enum class ESynth1OscType : uint8_t
{
	Sine = 0,
	Saw = 1,
	Triangle = 2,
	Square = 3,
	Noise = 4,
	Count = 5,
	ESynth1OscType_MAX = 6
};

// Object: Enum Synthesis.ESourceEffectDynamicsPeakMode
enum class ESourceEffectDynamicsPeakMode : uint8_t
{
	MeanSquared = 0,
	RootMeanSquared = 1,
	Peak = 2,
	Count = 3,
	ESourceEffectDynamicsPeakMode_MAX = 4
};

// Object: Enum Synthesis.ESourceEffectDynamicsProcessorType
enum class ESourceEffectDynamicsProcessorType : uint8_t
{
	Compressor = 0,
	Limiter = 1,
	Expander = 2,
	Gate = 3,
	Count = 4,
	ESourceEffectDynamicsProcessorType_MAX = 5
};

// Object: Enum Synthesis.EEnvelopeFollowerPeakMode
enum class EEnvelopeFollowerPeakMode : uint8_t
{
	MeanSquared = 0,
	RootMeanSquared = 1,
	Peak = 2,
	Count = 3,
	EEnvelopeFollowerPeakMode_MAX = 4
};

// Object: Enum Synthesis.ESourceEffectFilterParam
enum class ESourceEffectFilterParam : uint8_t
{
	FilterFrequency = 0,
	FilterResonance = 1,
	Count = 2,
	ESourceEffectFilterParam_MAX = 3
};

// Object: Enum Synthesis.ESourceEffectFilterType
enum class ESourceEffectFilterType : uint8_t
{
	LowPass = 0,
	HighPass = 1,
	BandPass = 2,
	BandStop = 3,
	Count = 4,
	ESourceEffectFilterType_MAX = 5
};

// Object: Enum Synthesis.ESourceEffectFilterCircuit
enum class ESourceEffectFilterCircuit : uint8_t
{
	OnePole = 0,
	StateVariable = 1,
	Ladder = 2,
	Count = 3,
	ESourceEffectFilterCircuit_MAX = 4
};

// Object: Enum Synthesis.EStereoChannelMode
enum class EStereoChannelMode : uint8_t
{
	MidSide = 0,
	LeftRight = 1,
	count = 2,
	EStereoChannelMode_MAX = 3
};

// Object: Enum Synthesis.EPhaserLFOType
enum class EPhaserLFOType : uint8_t
{
	Sine = 0,
	UpSaw = 1,
	DownSaw = 2,
	Square = 3,
	Triangle = 4,
	Exponential = 5,
	RandomSampleHold = 6,
	Count = 7,
	EPhaserLFOType_MAX = 8
};

// Object: Enum Synthesis.ERingModulatorTypeSourceEffect
enum class ERingModulatorTypeSourceEffect : uint8_t
{
	Sine = 0,
	Saw = 1,
	Triangle = 2,
	Square = 3,
	Count = 4,
	ERingModulatorTypeSourceEffect_MAX = 5
};

// Object: Enum Synthesis.EStereoDelayFiltertype
enum class EStereoDelayFiltertype : uint8_t
{
	Lowpass = 0,
	Highpass = 1,
	Bandpass = 2,
	Notch = 3,
	Count = 4,
	EStereoDelayFiltertype_MAX = 5
};

// Object: Enum Synthesis.EStereoDelaySourceEffect
enum class EStereoDelaySourceEffect : uint8_t
{
	Normal = 0,
	Cross = 1,
	PingPong = 2,
	Count = 3,
	EStereoDelaySourceEffect_MAX = 4
};

// Object: Enum Synthesis.ESubmixEffectConvolutionReverbBlockSize
enum class ESubmixEffectConvolutionReverbBlockSize : uint8_t
{
	BlockSize256 = 0,
	BlockSize512 = 1,
	BlockSize1024 = 2,
	ESubmixEffectConvolutionReverbBlockSize_MAX = 3
};

// Object: Enum Synthesis.ESubmixFilterAlgorithm
enum class ESubmixFilterAlgorithm : uint8_t
{
	OnePole = 0,
	StateVariable = 1,
	Ladder = 2,
	Count = 3,
	ESubmixFilterAlgorithm_MAX = 4
};

// Object: Enum Synthesis.ESubmixFilterType
enum class ESubmixFilterType : uint8_t
{
	LowPass = 0,
	HighPass = 1,
	BandPass = 2,
	BandStop = 3,
	Count = 4,
	ESubmixFilterType_MAX = 5
};

// Object: Enum Synthesis.ETapLineMode
enum class ETapLineMode : uint8_t
{
	SendToChannel = 0,
	Panning = 1,
	Disabled = 2,
	ETapLineMode_MAX = 3
};

// Object: Enum Synthesis.EGranularSynthSeekType
enum class EGranularSynthSeekType : uint8_t
{
	FromBeginning = 0,
	FromCurrentPosition = 1,
	Count = 2,
	EGranularSynthSeekType_MAX = 3
};

// Object: Enum Synthesis.EGranularSynthEnvelopeType
enum class EGranularSynthEnvelopeType : uint8_t
{
	Rectangular = 0,
	Triangle = 1,
	DownwardTriangle = 2,
	UpwardTriangle = 3,
	ExponentialDecay = 4,
	ExponentialIncrease = 5,
	Gaussian = 6,
	Hanning = 7,
	Lanczos = 8,
	Cosine = 9,
	CosineSquared = 10,
	Welch = 11,
	Blackman = 12,
	BlackmanHarris = 13,
	Count = 14,
	EGranularSynthEnvelopeType_MAX = 15
};

// Object: Enum Synthesis.CurveInterpolationType
enum class ECurveInterpolationType : uint8_t
{
	AUTOINTERP = 0,
	LINEAR = 1,
	CONSTANT = 2,
	CurveInterpolationType_MAX = 3
};

// Object: Enum Synthesis.ESamplePlayerSeekType
enum class ESamplePlayerSeekType : uint8_t
{
	FromBeginning = 0,
	FromCurrentPosition = 1,
	FromEnd = 2,
	Count = 3,
	ESamplePlayerSeekType_MAX = 4
};

// Object: Enum Synthesis.ESynthKnobSize
enum class ESynthKnobSize : uint8_t
{
	Medium = 0,
	Large = 1,
	Count = 2,
	ESynthKnobSize_MAX = 3
};

// Object: Enum Synthesis.ESynthSlateColorStyle
enum class ESynthSlateColorStyle : uint8_t
{
	Light = 0,
	Dark = 1,
	Count = 2,
	ESynthSlateColorStyle_MAX = 3
};

// Object: Enum Synthesis.ESynthSlateSizeType
enum class ESynthSlateSizeType : uint8_t
{
	Small = 0,
	Medium = 1,
	Large = 2,
	Count = 3,
	ESynthSlateSizeType_MAX = 4
};

// Package: AudioSynesthesia
// Enums: 3

// Object: Enum AudioSynesthesia.EConstantQFFTSizeEnum
enum class EConstantQFFTSizeEnum : uint8_t
{
	Min = 0,
	XXSmall = 1,
	XSmall = 2,
	Small = 3,
	Medium = 4,
	Large = 5,
	XLarge = 6,
	XXLarge = 7,
	Max = 8
};

// Object: Enum AudioSynesthesia.EConstantQNormalizationEnum
enum class EConstantQNormalizationEnum : uint8_t
{
	EqualEuclideanNorm = 0,
	EqualEnergy = 1,
	EqualAmplitude = 2,
	EConstantQNormalizationEnum_MAX = 3
};

// Object: Enum AudioSynesthesia.ELoudnessNRTCurveTypeEnum
enum class ELoudnessNRTCurveTypeEnum : uint8_t
{
	A = 0,
	B = 1,
	C = 2,
	D = 3,
	None = 4,
	ELoudnessNRTCurveTypeEnum_MAX = 5
};

// Package: InputCore
// Enums: 4

// Object: Enum InputCore.ETouchIndex
enum class ETouchIndex : uint8_t
{
	Touch1 = 0,
	Touch2 = 1,
	Touch3 = 2,
	Touch4 = 3,
	Touch5 = 4,
	Touch6 = 5,
	Touch7 = 6,
	Touch8 = 7,
	Touch9 = 8,
	Touch10 = 9,
	CursorPointerIndex = 10,
	MAX_TOUCHES = 11,
	ETouchIndex_MAX = 12
};

// Object: Enum InputCore.EControllerHand
enum class EControllerHand : uint8_t
{
	Left = 0,
	Right = 1,
	AnyHand = 2,
	Pad = 3,
	ExternalCamera = 4,
	Gun = 5,
	Special = 6,
	Special = 7,
	Special = 8,
	Special = 9,
	Special = 10,
	Special = 11,
	Special = 12,
	Special = 13,
	Special = 14,
	Special = 15,
	Special = 16,
	ControllerHand_Count = 17,
	EControllerHand_MAX = 18
};

// Object: Enum InputCore.ETouchType
enum class ETouchType : uint8_t
{
	Began = 0,
	Moved = 1,
	Stationary = 2,
	ForceChanged = 3,
	FirstMove = 4,
	Ended = 5,
	NumTypes = 6,
	ETouchType_MAX = 7
};

// Object: Enum InputCore.EConsoleForGamepadLabels
enum class EConsoleForGamepadLabels : uint8_t
{
	None = 0,
	XBoxOne = 1,
	PS4 = 2,
	EConsoleForGamepadLabels_MAX = 3
};

// Package: Slate
// Enums: 21

// Object: Enum Slate.ETextJustify
enum class ETextJustify : uint8_t
{
	Left = 0,
	Center = 1,
	Right = 2,
	ETextJustify_MAX = 3
};

// Object: Enum Slate.ETextFlowDirection
enum class ETextFlowDirection : uint8_t
{
	Auto = 0,
	LeftToRight = 1,
	RightToLeft = 2,
	ETextFlowDirection_MAX = 3
};

// Object: Enum Slate.EVirtualKeyboardDismissAction
enum class EVirtualKeyboardDismissAction : uint8_t
{
	TextChangeOnDismiss = 0,
	TextCommitOnAccept = 1,
	TextCommitOnDismiss = 2,
	EVirtualKeyboardDismissAction_MAX = 3
};

// Object: Enum Slate.EVirtualKeyboardTrigger
enum class EVirtualKeyboardTrigger : uint8_t
{
	OnFocusByPointer = 0,
	OnAllFocusEvents = 1,
	EVirtualKeyboardTrigger_MAX = 2
};

// Object: Enum Slate.EReturnKeyType
enum class EReturnKeyType : uint8_t
{
	ReturnKey_Default = 0,
	ReturnKey_Go = 1,
	ReturnKey_Join = 2,
	ReturnKey_Next = 3,
	ReturnKey_Search = 4,
	ReturnKey_Send = 5,
	ReturnKey_Done = 6,
	ReturnKey_Continue = 7,
	ReturnKey_MAX = 8
};

// Object: Enum Slate.ETextWrappingPolicy
enum class ETextWrappingPolicy : uint8_t
{
	DefaultWrapping = 0,
	AllowPerCharacterWrapping = 1,
	BreakingWrapping_Continue = 2,
	ETextWrappingPolicy_MAX = 3
};

// Object: Enum Slate.ETableViewMode
enum class ETableViewMode : uint8_t
{
	List = 0,
	Tile = 1,
	Tree = 2,
	ETableViewMode_MAX = 3
};

// Object: Enum Slate.ESelectionMode
enum class ESelectionMode : uint8_t
{
	None = 0,
	Single = 1,
	SingleToggle = 2,
	Multi = 3,
	ESelectionMode_MAX = 4
};

// Object: Enum Slate.EMultiBlockType
enum class EMultiBlockType : uint8_t
{
	None = 0,
	ButtonRow = 1,
	EditableText = 2,
	Heading = 3,
	MenuEntry = 4,
	Separator = 5,
	ToolBarButton = 6,
	ToolBarComboButton = 7,
	Widget = 8,
	EMultiBlockType_MAX = 9
};

// Object: Enum Slate.EMultiBoxType
enum class EMultiBoxType : uint8_t
{
	MenuBar = 0,
	ToolBar = 1,
	VerticalToolBar = 2,
	UniformToolBar = 3,
	Menu = 4,
	ButtonRow = 5,
	EMultiBoxType_MAX = 6
};

// Object: Enum Slate.EProgressBarFillType
enum class EProgressBarFillType : uint8_t
{
	LeftToRight = 0,
	RightToLeft = 1,
	FillFromCenter = 2,
	TopToBottom = 3,
	BottomToTop = 4,
	EProgressBarFillType_MAX = 5
};

// Object: Enum Slate.EStretch
enum class EStretch : uint8_t
{
	None = 0,
	Fill = 1,
	ScaleToFit = 2,
	ScaleToFitX = 3,
	ScaleToFitY = 4,
	ScaleToFill = 5,
	ScaleBySafeZone = 6,
	UserSpecified = 7,
	EStretch_MAX = 8
};

// Object: Enum Slate.EStretchDirection
enum class EStretchDirection : uint8_t
{
	Both = 0,
	DownOnly = 1,
	UpOnly = 2,
	EStretchDirection_MAX = 3
};

// Object: Enum Slate.EScrollWhenFocusChanges
enum class EScrollWhenFocusChanges : uint8_t
{
	NoScroll = 0,
	InstantScroll = 1,
	AnimatedScroll = 2,
	EScrollWhenFocusChanges_MAX = 3
};

// Object: Enum Slate.EDescendantScrollDestination
enum class EDescendantScrollDestination : uint8_t
{
	IntoView = 0,
	TopOrLeft = 1,
	Center = 2,
	EDescendantScrollDestination_MAX = 3
};

// Object: Enum Slate.EListItemAlignment
enum class EListItemAlignment : uint8_t
{
	EvenlyDistributed = 0,
	EvenlySize = 1,
	EvenlyWide = 2,
	LeftAligned = 3,
	RightAligned = 4,
	CenterAligned = 5,
	Fill = 6,
	EListItemAlignment_MAX = 7
};

// Object: Enum Slate.EText3DBlendMode
enum class EText3DBlendMode : uint8_t
{
	Original = 0,
	WriteColorAndAlpha = 1,
	BlendWithLowLayer = 2,
	EText3DBlendMode_MAX = 3
};

// Object: Enum Slate.ETextTransformPolicy
enum class ETextTransformPolicy : uint8_t
{
	None = 0,
	ToLower = 1,
	ToUpper = 2,
	ETextTransformPolicy_MAX = 3
};

// Object: Enum Slate.ECustomizedToolMenuVisibility
enum class ECustomizedToolMenuVisibility : uint8_t
{
	None = 0,
	Visible = 1,
	Hidden = 2,
	ECustomizedToolMenuVisibility_MAX = 3
};

// Object: Enum Slate.EMultipleKeyBindingIndex
enum class EMultipleKeyBindingIndex : uint8_t
{
	Primary = 0,
	Secondary = 1,
	NumChords = 2,
	EMultipleKeyBindingIndex_MAX = 3
};

// Object: Enum Slate.EUserInterfaceActionType
enum class EUserInterfaceActionType : uint8_t
{
	None = 0,
	Button = 1,
	ToggleButton = 2,
	RadioButton = 3,
	Check = 4,
	CollapsedButton = 5,
	EUserInterfaceActionType_MAX = 6
};

// Package: ImageWriteQueue
// Enums: 1

// Object: Enum ImageWriteQueue.EDesiredImageFormat
enum class EDesiredImageFormat : uint8_t
{
	PNG = 0,
	JPG = 1,
	BMP = 2,
	EXR = 3,
	EDesiredImageFormat_MAX = 4
};

// Package: MaterialShaderQualitySettings
// Enums: 1

// Object: Enum MaterialShaderQualitySettings.EMobileShadowQuality
enum class EMobileShadowQuality : uint8_t
{
	NoFiltering = 0,
	PCF_1x1 = 1,
	PCF_2x2 = 2,
	PCF_3x3 = 3,
	EMobileShadowQuality_MAX = 4
};

// Package: EngineSettings
// Enums: 4

// Object: Enum EngineSettings.ESubLevelStripMode
enum class ESubLevelStripMode : uint8_t
{
	ExactClass = 0,
	IsChildOf = 1,
	ESubLevelStripMode_MAX = 2
};

// Object: Enum EngineSettings.EFourPlayerSplitScreenType
enum class EFourPlayerSplitScreenType : uint8_t
{
	Grid = 0,
	Vertical = 1,
	Horizontal = 2,
	EFourPlayerSplitScreenType_MAX = 3
};

// Object: Enum EngineSettings.EThreePlayerSplitScreenType
enum class EThreePlayerSplitScreenType : uint8_t
{
	FavorTop = 0,
	FavorBottom = 1,
	Vertical = 2,
	Horizontal = 3,
	EThreePlayerSplitScreenType_MAX = 4
};

// Object: Enum EngineSettings.ETwoPlayerSplitScreenType
enum class ETwoPlayerSplitScreenType : uint8_t
{
	Horizontal = 0,
	Vertical = 1,
	ETwoPlayerSplitScreenType_MAX = 2
};

// Package: MRMesh
// Enums: 1

// Object: Enum MRMesh.EMeshTrackerVertexColorMode
enum class EMeshTrackerVertexColorMode : uint8_t
{
	None = 0,
	Confidence = 1,
	Block = 2,
	EMeshTrackerVertexColorMode_MAX = 3
};

// Package: AugmentedReality
// Enums: 41

// Object: Enum AugmentedReality.EARTrackingState
enum class EARTrackingState : uint8_t
{
	Unknown = 0,
	Tracking = 1,
	NotTracking = 2,
	StoppedTracking = 3,
	EARTrackingState_MAX = 4
};

// Object: Enum AugmentedReality.EGeoAnchorComponentDebugMode
enum class EGeoAnchorComponentDebugMode : uint8_t
{
	None = 0,
	ShowGeoData = 1,
	EGeoAnchorComponentDebugMode_MAX = 2
};

// Object: Enum AugmentedReality.EPoseComponentDebugMode
enum class EPoseComponentDebugMode : uint8_t
{
	None = 0,
	ShowSkeleton = 1,
	EPoseComponentDebugMode_MAX = 2
};

// Object: Enum AugmentedReality.EQRCodeComponentDebugMode
enum class EQRCodeComponentDebugMode : uint8_t
{
	None = 0,
	ShowQRCode = 1,
	EQRCodeComponentDebugMode_MAX = 2
};

// Object: Enum AugmentedReality.EImageComponentDebugMode
enum class EImageComponentDebugMode : uint8_t
{
	None = 0,
	ShowDetectedImage = 1,
	EImageComponentDebugMode_MAX = 2
};

// Object: Enum AugmentedReality.EARFaceTransformMixing
enum class EARFaceTransformMixing : uint8_t
{
	ComponentOnly = 0,
	ComponentLocationTrackedRotation = 1,
	ComponentWithTracked = 2,
	TrackingOnly = 3,
	EARFaceTransformMixing_MAX = 4
};

// Object: Enum AugmentedReality.EFaceComponentDebugMode
enum class EFaceComponentDebugMode : uint8_t
{
	None = 0,
	ShowEyeVectors = 1,
	ShowFaceMesh = 2,
	EFaceComponentDebugMode_MAX = 3
};

// Object: Enum AugmentedReality.EPlaneComponentDebugMode
enum class EPlaneComponentDebugMode : uint8_t
{
	None = 0,
	ShowNetworkRole = 1,
	ShowClassification = 2,
	EPlaneComponentDebugMode_MAX = 3
};

// Object: Enum AugmentedReality.EARSessionConfigFlags
enum class EARSessionConfigFlags : uint8_t
{
	None = 0,
	GenerateMeshData = 1,
	RenderMeshDataInWireframe = 2,
	GenerateCollisionForMeshData = 4,
	GenerateNavMeshForMeshData = 8,
	UseMeshDataForOcclusion = 16,
	EARSessionConfigFlags_MAX = 17
};

// Object: Enum AugmentedReality.EARServicePermissionRequestResult
enum class EARServicePermissionRequestResult : uint8_t
{
	Granted = 0,
	Denied = 1,
	EARServicePermissionRequestResult_MAX = 2
};

// Object: Enum AugmentedReality.EARServiceInstallRequestResult
enum class EARServiceInstallRequestResult : uint8_t
{
	Installed = 0,
	DeviceNotCompatible = 1,
	UserDeclinedInstallation = 2,
	FatalError = 3,
	EARServiceInstallRequestResult_MAX = 4
};

// Object: Enum AugmentedReality.EARServiceAvailability
enum class EARServiceAvailability : uint8_t
{
	UnknownError = 0,
	UnknownChecking = 1,
	UnknownTimedOut = 2,
	UnsupportedDeviceNotCapable = 3,
	SupportedNotInstalled = 4,
	SupportedVersionTooOld = 5,
	SupportedInstalled = 6,
	EARServiceAvailability_MAX = 7
};

// Object: Enum AugmentedReality.EARGeoTrackingAccuracy
enum class EARGeoTrackingAccuracy : uint8_t
{
	Undetermined = 0,
	Low = 1,
	Medium = 2,
	High = 3,
	EARGeoTrackingAccuracy_MAX = 4
};

// Object: Enum AugmentedReality.EARGeoTrackingStateReason
enum class EARGeoTrackingStateReason : uint8_t
{
	None = 0,
	NotAvailableAtLocation = 1,
	NeedLocationPermissions = 2,
	DevicePointedTooLow = 3,
	WorldTrackingUnstable = 4,
	WaitingForLocation = 5,
	GeoDataNotLoaded = 6,
	VisualLocalizationFailed = 7,
	WaitingForAvailabilityCheck = 8,
	EARGeoTrackingStateReason_MAX = 9
};

// Object: Enum AugmentedReality.EARGeoTrackingState
enum class EARGeoTrackingState : uint8_t
{
	Initializing = 0,
	Localized = 1,
	Localizing = 2,
	NotAvailable = 3,
	EARGeoTrackingState_MAX = 4
};

// Object: Enum AugmentedReality.EARSceneReconstruction
enum class EARSceneReconstruction : uint8_t
{
	None = 0,
	MeshOnly = 1,
	MeshWithClassification = 2,
	EARSceneReconstruction_MAX = 3
};

// Object: Enum AugmentedReality.EARSessionTrackingFeature
enum class EARSessionTrackingFeature : uint8_t
{
	None = 0,
	PoseDetection2D = 1,
	PersonSegmentation = 2,
	PersonSegmentationWithDepth = 3,
	SceneDepth = 4,
	SmoothedSceneDepth = 5,
	EARSessionTrackingFeature_MAX = 6
};

// Object: Enum AugmentedReality.EARFaceTrackingUpdate
enum class EARFaceTrackingUpdate : uint8_t
{
	CurvesAndGeo = 0,
	CurvesOnly = 1,
	EARFaceTrackingUpdate_MAX = 2
};

// Object: Enum AugmentedReality.EAREnvironmentCaptureProbeType
enum class EAREnvironmentCaptureProbeType : uint8_t
{
	None = 0,
	Manual = 1,
	Automatic = 2,
	EAREnvironmentCaptureProbeType_MAX = 3
};

// Object: Enum AugmentedReality.EARFrameSyncMode
enum class EARFrameSyncMode : uint8_t
{
	SyncTickWithCameraImage = 0,
	SyncTickWithoutCameraImage = 1,
	EARFrameSyncMode_MAX = 2
};

// Object: Enum AugmentedReality.EARLightEstimationMode
enum class EARLightEstimationMode : uint8_t
{
	None = 0,
	AmbientLightEstimate = 1,
	DirectionalLightEstimate = 2,
	EARLightEstimationMode_MAX = 3
};

// Object: Enum AugmentedReality.EARPlaneDetectionMode
enum class EARPlaneDetectionMode : uint8_t
{
	None = 0,
	HorizontalPlaneDetection = 1,
	VerticalPlaneDetection = 2,
	EARPlaneDetectionMode_MAX = 3
};

// Object: Enum AugmentedReality.EARSessionType
enum class EARSessionType : uint8_t
{
	None = 0,
	Orientation = 1,
	World = 2,
	Face = 3,
	Image = 4,
	ObjectScanning = 5,
	PoseTracking = 6,
	GeoTracking = 7,
	EARSessionType_MAX = 8
};

// Object: Enum AugmentedReality.EARWorldAlignment
enum class EARWorldAlignment : uint8_t
{
	Gravity = 0,
	GravityAndHeading = 1,
	Camera = 2,
	EARWorldAlignment_MAX = 3
};

// Object: Enum AugmentedReality.EARDepthAccuracy
enum class EARDepthAccuracy : uint8_t
{
	Unkown = 0,
	Approximate = 1,
	Accurate = 2,
	EARDepthAccuracy_MAX = 3
};

// Object: Enum AugmentedReality.EARDepthQuality
enum class EARDepthQuality : uint8_t
{
	Unkown = 0,
	Low = 1,
	High = 2,
	EARDepthQuality_MAX = 3
};

// Object: Enum AugmentedReality.EARTextureType
enum class EARTextureType : uint8_t
{
	Unknown = 0,
	CameraImage = 1,
	CameraDepth = 2,
	EnvironmentCapture = 3,
	PersonSegmentationImage = 4,
	PersonSegmentationDepth = 5,
	SceneDepthMap = 6,
	SceneDepthConfidenceMap = 7,
	EARTextureType_MAX = 8
};

// Object: Enum AugmentedReality.EAREye
enum class EAREye : uint8_t
{
	LeftEye = 0,
	RightEye = 1,
	EAREye_MAX = 2
};

// Object: Enum AugmentedReality.EARFaceBlendShape
enum class EARFaceBlendShape : uint8_t
{
	EyeBlinkLeft = 0,
	EyeLookDownLeft = 1,
	EyeLookInLeft = 2,
	EyeLookOutLeft = 3,
	EyeLookUpLeft = 4,
	EyeSquintLeft = 5,
	EyeWideLeft = 6,
	EyeBlinkRight = 7,
	EyeLookDownRight = 8,
	EyeLookInRight = 9,
	EyeLookOutRight = 10,
	EyeLookUpRight = 11,
	EyeSquintRight = 12,
	EyeWideRight = 13,
	JawForward = 14,
	JawLeft = 15,
	JawRight = 16,
	JawOpen = 17,
	MouthClose = 18,
	MouthFunnel = 19,
	MouthPucker = 20,
	MouthLeft = 21,
	MouthRight = 22,
	MouthSmileLeft = 23,
	MouthSmileRight = 24,
	MouthFrownLeft = 25,
	MouthFrownRight = 26,
	MouthDimpleLeft = 27,
	MouthDimpleRight = 28,
	MouthStretchLeft = 29,
	MouthStretchRight = 30,
	MouthRollLower = 31,
	MouthRollUpper = 32,
	MouthShrugLower = 33,
	MouthShrugUpper = 34,
	MouthPressLeft = 35,
	MouthPressRight = 36,
	MouthLowerDownLeft = 37,
	MouthLowerDownRight = 38,
	MouthUpperUpLeft = 39,
	MouthUpperUpRight = 40,
	BrowDownLeft = 41,
	BrowDownRight = 42,
	BrowInnerUp = 43,
	BrowOuterUpLeft = 44,
	BrowOuterUpRight = 45,
	CheekPuff = 46,
	CheekSquintLeft = 47,
	CheekSquintRight = 48,
	NoseSneerLeft = 49,
	NoseSneerRight = 50,
	TongueOut = 51,
	HeadYaw = 52,
	HeadPitch = 53,
	HeadRoll = 54,
	LeftEyeYaw = 55,
	LeftEyePitch = 56,
	LeftEyeRoll = 57,
	RightEyeYaw = 58,
	RightEyePitch = 59,
	RightEyeRoll = 60,
	MAX = 61
};

// Object: Enum AugmentedReality.EARFaceTrackingDirection
enum class EARFaceTrackingDirection : uint8_t
{
	FaceRelative = 0,
	FaceMirrored = 1,
	EARFaceTrackingDirection_MAX = 2
};

// Object: Enum AugmentedReality.EARCandidateImageOrientation
enum class EARCandidateImageOrientation : uint8_t
{
	Landscape = 0,
	Portrait = 1,
	EARCandidateImageOrientation_MAX = 2
};

// Object: Enum AugmentedReality.EARAltitudeSource
enum class EARAltitudeSource : uint8_t
{
	Precise = 0,
	Coarse = 1,
	UserDefined = 2,
	Unknown = 3,
	EARAltitudeSource_MAX = 4
};

// Object: Enum AugmentedReality.EARJointTransformSpace
enum class EARJointTransformSpace : uint8_t
{
	Model = 0,
	ParentJoint = 1,
	EARJointTransformSpace_MAX = 2
};

// Object: Enum AugmentedReality.EARObjectClassification
enum class EARObjectClassification : uint8_t
{
	NotApplicable = 0,
	Unknown = 1,
	Wall = 2,
	Ceiling = 3,
	Floor = 4,
	Table = 5,
	Seat = 6,
	Face = 7,
	Image = 8,
	World = 9,
	SceneObject = 10,
	HandMesh = 11,
	Door = 12,
	Window = 13,
	EARObjectClassification_MAX = 14
};

// Object: Enum AugmentedReality.EARPlaneOrientation
enum class EARPlaneOrientation : uint8_t
{
	Horizontal = 0,
	Vertical = 1,
	Diagonal = 2,
	EARPlaneOrientation_MAX = 3
};

// Object: Enum AugmentedReality.EARWorldMappingState
enum class EARWorldMappingState : uint8_t
{
	NotAvailable = 0,
	StillMappingNotRelocalizable = 1,
	StillMappingRelocalizable = 2,
	Mapped = 3,
	EARWorldMappingState_MAX = 4
};

// Object: Enum AugmentedReality.EARSessionStatus
enum class EARSessionStatus : uint8_t
{
	NotStarted = 0,
	Running = 1,
	NotSupported = 2,
	FatalError = 3,
	PermissionNotGranted = 4,
	UnsupportedConfiguration = 5,
	Other = 6,
	EARSessionStatus_MAX = 7
};

// Object: Enum AugmentedReality.EARTrackingQualityReason
enum class EARTrackingQualityReason : uint8_t
{
	None = 0,
	Initializing = 1,
	Relocalizing = 2,
	ExcessiveMotion = 3,
	InsufficientFeatures = 4,
	InsufficientLight = 5,
	BadState = 6,
	EARTrackingQualityReason_MAX = 7
};

// Object: Enum AugmentedReality.EARTrackingQuality
enum class EARTrackingQuality : uint8_t
{
	NotTracking = 0,
	OrientationOnly = 1,
	OrientationAndPosition = 2,
	EARTrackingQuality_MAX = 3
};

// Object: Enum AugmentedReality.EARLineTraceChannels
enum class EARLineTraceChannels : uint8_t
{
	None = 0,
	FeaturePoint = 1,
	GroundPlane = 2,
	PlaneUsingExtent = 4,
	PlaneUsingBoundaryPolygon = 8,
	EARLineTraceChannels_MAX = 9
};

// Object: Enum AugmentedReality.EARCaptureType
enum class EARCaptureType : uint8_t
{
	Camera = 0,
	QRCode = 1,
	SpatialMapping = 2,
	SceneUnderstanding = 3,
	EARCaptureType_MAX = 4
};

// Package: HeadMountedDisplay
// Enums: 11

// Object: Enum HeadMountedDisplay.EXRVisualType
enum class EXRVisualType : uint8_t
{
	Controller = 0,
	Hand = 1,
	EXRVisualType_MAX = 2
};

// Object: Enum HeadMountedDisplay.EHandKeypoint
enum class EHandKeypoint : uint8_t
{
	Palm = 0,
	Wrist = 1,
	ThumbMetacarpal = 2,
	ThumbProximal = 3,
	ThumbDistal = 4,
	ThumbTip = 5,
	IndexMetacarpal = 6,
	IndexProximal = 7,
	IndexIntermediate = 8,
	IndexDistal = 9,
	IndexTip = 10,
	MiddleMetacarpal = 11,
	MiddleProximal = 12,
	MiddleIntermediate = 13,
	MiddleDistal = 14,
	MiddleTip = 15,
	RingMetacarpal = 16,
	RingProximal = 17,
	RingIntermediate = 18,
	RingDistal = 19,
	RingTip = 20,
	LittleMetacarpal = 21,
	LittleProximal = 22,
	LittleIntermediate = 23,
	LittleDistal = 24,
	LittleTip = 25,
	EHandKeypoint_MAX = 26
};

// Object: Enum HeadMountedDisplay.EXRTrackedDeviceType
enum class EXRTrackedDeviceType : uint16_t
{
	HeadMountedDisplay = 0,
	Controller = 1,
	TrackingReference = 2,
	Other = 3,
	Invalid = 254,
	Any = 255,
	EXRTrackedDeviceType_MAX = 256
};

// Object: Enum HeadMountedDisplay.ESpectatorScreenMode
enum class ESpectatorScreenMode : uint8_t
{
	Disabled = 0,
	SingleEyeLetterboxed = 1,
	Undistorted = 2,
	Distorted = 3,
	SingleEye = 4,
	SingleEyeCroppedToFill = 5,
	Texture = 6,
	TexturePlusEye = 7,
	ESpectatorScreenMode_MAX = 8
};

// Object: Enum HeadMountedDisplay.EXRSystemFlags
enum class EXRSystemFlags : uint8_t
{
	NoFlags = 0,
	IsAR = 1,
	IsTablet = 2,
	IsHeadMounted = 4,
	SupportsHandTracking = 8,
	EXRSystemFlags_MAX = 9
};

// Object: Enum HeadMountedDisplay.EXRDeviceConnectionResult
enum class EXRDeviceConnectionResult : uint8_t
{
	NoTrackingSystem = 0,
	FeatureNotSupported = 1,
	NoValidViewport = 2,
	MiscFailure = 3,
	Success = 4,
	EXRDeviceConnectionResult_MAX = 5
};

// Object: Enum HeadMountedDisplay.EHMDWornState
enum class EHMDWornState : uint8_t
{
	Unknown = 0,
	Worn = 1,
	NotWorn = 2,
	EHMDWornState_MAX = 3
};

// Object: Enum HeadMountedDisplay.EHMDTrackingOrigin
enum class EHMDTrackingOrigin : uint8_t
{
	Floor = 0,
	Eye = 1,
	Stage = 2,
	Unbounded = 3,
	EHMDTrackingOrigin_MAX = 4
};

// Object: Enum HeadMountedDisplay.EOrientPositionSelector
enum class EOrientPositionSelector : uint8_t
{
	Orientation = 0,
	Position = 1,
	OrientationAndPosition = 2,
	EOrientPositionSelector_MAX = 3
};

// Object: Enum HeadMountedDisplay.ETrackingStatus
enum class ETrackingStatus : uint8_t
{
	NotTracked = 0,
	InertialOnly = 1,
	Tracked = 2,
	ETrackingStatus_MAX = 3
};

// Object: Enum HeadMountedDisplay.ESpatialInputGestureAxis
enum class ESpatialInputGestureAxis : uint8_t
{
	None = 0,
	Manipulation = 1,
	Navigation = 2,
	NavigationRails = 3,
	ESpatialInputGestureAxis_MAX = 4
};

// Package: Foliage
// Enums: 7

// Object: Enum Foliage.EFoliageScaling
enum class EFoliageScaling : uint8_t
{
	Uniform = 0,
	Free = 1,
	LockXY = 2,
	LockXZ = 3,
	LockYZ = 4,
	EFoliageScaling_MAX = 5
};

// Object: Enum Foliage.EFoliageUsage
enum class EFoliageUsage : uint8_t
{
	USAGE_NONE = 0,
	USAGE_FARM = 1,
	USAGE_GRASSSEA = 2,
	USAGE_MAX = 3
};

// Object: Enum Foliage.FoliageInstanceType
enum class EFoliageInstanceType : uint8_t
{
	FOLIAGEINSTANCE_TREE = 0,
	FOLIAGEINSTANCE_GRASS = 1,
	FOLIAGEINSTANCE_PLANT = 2,
	FOLIAGEINSTANCE_ROCK = 3,
	FOLIAGEINSTANCE_MAX = 4
};

// Object: Enum Foliage.EVertexColorMaskChannel
enum class EVertexColorMaskChannel : uint8_t
{
	Red = 0,
	Green = 1,
	Blue = 2,
	Alpha = 3,
	MAX_None = 4,
	EVertexColorMaskChannel_MAX = 5
};

// Object: Enum Foliage.FoliageVertexColorMask
enum class EFoliageVertexColorMask : uint8_t
{
	FOLIAGEVERTEXCOLORMASK_Disabled = 0,
	FOLIAGEVERTEXCOLORMASK_Red = 1,
	FOLIAGEVERTEXCOLORMASK_Green = 2,
	FOLIAGEVERTEXCOLORMASK_Blue = 3,
	FOLIAGEVERTEXCOLORMASK_Alpha = 4,
	FOLIAGEVERTEXCOLORMASK_MAX = 5
};

// Object: Enum Foliage.ESimulationQuery
enum class ESimulationQuery : uint8_t
{
	None = 0,
	CollisionOverlap = 1,
	ShadeOverlap = 2,
	AnyOverlap = 3,
	ESimulationQuery_MAX = 4
};

// Object: Enum Foliage.ESimulationOverlap
enum class ESimulationOverlap : uint8_t
{
	CollisionOverlap = 0,
	ShadeOverlap = 1,
	None = 2,
	ESimulationOverlap_MAX = 3
};

// Package: Landscape
// Enums: 16

// Object: Enum Landscape.ELandscapeBlendMode
enum class ELandscapeBlendMode : uint8_t
{
	LSBM_AdditiveBlend = 0,
	LSBM_AlphaBlend = 1,
	LSBM_MAX = 2
};

// Object: Enum Landscape.EACMWeightType
enum class EACMWeightType : uint8_t
{
	EWT_Splat = 0,
	EWT_Logic = 1,
	EWT_Blend0 = 2,
	EWT_Blend1 = 3,
	EWT_MAX = 4
};

// Object: Enum Landscape.ELandscapeSetupErrors
enum class ELandscapeSetupErrors : uint8_t
{
	LSE_None = 0,
	LSE_NoLandscapeInfo = 1,
	LSE_CollsionXY = 2,
	LSE_NoLayerInfo = 3,
	LSE_MAX = 4
};

// Object: Enum Landscape.ELandscapeClearMode
enum class ELandscapeClearMode : uint8_t
{
	Clear_Weightmap = 1,
	Clear_Heightmap = 2,
	Clear_All = 3,
	Clear_MAX = 4
};

// Object: Enum Landscape.ELandscapeGizmoType
enum class ELandscapeGizmoType : uint8_t
{
	LGT_None = 0,
	LGT_Height = 1,
	LGT_Weight = 2,
	LGT_MAX = 3
};

// Object: Enum Landscape.EGrassScaling
enum class EGrassScaling : uint8_t
{
	Uniform = 0,
	Free = 1,
	LockXY = 2,
	EGrassScaling_MAX = 3
};

// Object: Enum Landscape.AcmLandscapeLayerType
enum class EAcmLandscapeLayerType : uint8_t
{
	Render = 0,
	Biome0 = 1,
	Biome1 = 2,
	Biome2 = 3,
	Logic0 = 4,
	Logic1 = 5,
	Logic2 = 6,
	Logic3 = 7,
	Blend0 = 8,
	Blend1 = 9,
	AcmLandscapeLayerType_MAX = 10
};

// Object: Enum Landscape.ESplineModulationColorMask
enum class ESplineModulationColorMask : uint8_t
{
	Red = 0,
	Green = 1,
	Blue = 2,
	Alpha = 3,
	ESplineModulationColorMask_MAX = 4
};

// Object: Enum Landscape.ELandscapeLODFalloff
enum class ELandscapeLODFalloff : uint8_t
{
	Linear = 0,
	SquareRoot = 1,
	ELandscapeLODFalloff_MAX = 2
};

// Object: Enum Landscape.ELandscapeLayerDisplayMode
enum class ELandscapeLayerDisplayMode : uint8_t
{
	Default = 0,
	Alphabetical = 1,
	UserSpecific = 2,
	ELandscapeLayerDisplayMode_MAX = 3
};

// Object: Enum Landscape.ELandscapeLayerPaintingRestriction
enum class ELandscapeLayerPaintingRestriction : uint8_t
{
	None = 0,
	UseMaxLayers = 1,
	ExistingOnly = 2,
	UseComponentWhitelist = 3,
	ProtectSpecialLayers = 4,
	ELandscapeLayerPaintingRestriction_MAX = 5
};

// Object: Enum Landscape.ELandscapeImportAlphamapType
enum class ELandscapeImportAlphamapType : uint8_t
{
	Additive = 0,
	Layered = 1,
	ELandscapeImportAlphamapType_MAX = 2
};

// Object: Enum Landscape.LandscapeSplineMeshOrientation
enum class ELandscapeSplineMeshOrientation : uint8_t
{
	LSMO_XUp = 0,
	LSMO_YUp = 1,
	LSMO_MAX = 2
};

// Object: Enum Landscape.ELandscapeLayerBlendType
enum class ELandscapeLayerBlendType : uint8_t
{
	LB_WeightBlend = 0,
	LB_AlphaBlend = 1,
	LB_HeightBlend = 2,
	LB_MAX = 3
};

// Object: Enum Landscape.ELandscapeCustomizedCoordType
enum class ELandscapeCustomizedCoordType : uint8_t
{
	LCCT_None = 0,
	LCCT_CustomUV0 = 1,
	LCCT_CustomUV1 = 2,
	LCCT_CustomUV2 = 3,
	LCCT_WeightMapUV = 4,
	LCCT_MAX = 5
};

// Object: Enum Landscape.ETerrainCoordMappingType
enum class ETerrainCoordMappingType : uint8_t
{
	TCMT_Auto = 0,
	TCMT_XY = 1,
	TCMT_XZ = 2,
	TCMT_YZ = 3,
	TCMT_MAX = 4
};

// Package: AnimationCore
// Enums: 2

// Object: Enum AnimationCore.ETransformConstraintType
enum class ETransformConstraintType : uint8_t
{
	Translation = 0,
	Rotation = 1,
	Scale = 2,
	Parent = 3,
	ETransformConstraintType_MAX = 4
};

// Object: Enum AnimationCore.EConstraintType
enum class EConstraintType : uint8_t
{
	Transform = 0,
	Aim = 1,
	MAX = 2
};

// Package: GameplayTags
// Enums: 5

// Object: Enum GameplayTags.EGameplayTagQueryExprType
enum class EGameplayTagQueryExprType : uint8_t
{
	Undefined = 0,
	AnyTagsMatch = 1,
	AllTagsMatch = 2,
	NoTagsMatch = 3,
	AnyExprMatch = 4,
	AllExprMatch = 5,
	NoExprMatch = 6,
	EGameplayTagQueryExprType_MAX = 7
};

// Object: Enum GameplayTags.EGameplayContainerMatchType
enum class EGameplayContainerMatchType : uint8_t
{
	Any = 0,
	All = 1,
	EGameplayContainerMatchType_MAX = 2
};

// Object: Enum GameplayTags.EGameplayTagMatchType
enum class EGameplayTagMatchType : uint8_t
{
	Explicit = 0,
	IncludeParentTags = 1,
	EGameplayTagMatchType_MAX = 2
};

// Object: Enum GameplayTags.EGameplayTagSelectionType
enum class EGameplayTagSelectionType : uint8_t
{
	None = 0,
	NonRestrictedOnly = 1,
	RestrictedOnly = 2,
	All = 3,
	EGameplayTagSelectionType_MAX = 4
};

// Object: Enum GameplayTags.EGameplayTagSourceType
enum class EGameplayTagSourceType : uint8_t
{
	Native = 0,
	DefaultTagList = 1,
	TagList = 2,
	RestrictedTagList = 3,
	DataTable = 4,
	Invalid = 5,
	EGameplayTagSourceType_MAX = 6
};

// Package: MeshDescription
// Enums: 1

// Object: Enum MeshDescription.EComputeNTBsOptions
enum class EComputeNTBsOptions : uint8_t
{
	None = 0,
	Normals = 1,
	Tangents = 2,
	WeightedNTBs = 4,
	EComputeNTBsOptions_MAX = 5
};

// Package: PropertyAccess
// Enums: 4

// Object: Enum PropertyAccess.EPropertyAccessCopyBatch
enum class EPropertyAccessCopyBatch : uint8_t
{
	InternalUnbatched = 0,
	ExternalUnbatched = 1,
	InternalBatched = 2,
	ExternalBatched = 3,
	Count = 4,
	EPropertyAccessCopyBatch_MAX = 5
};

// Object: Enum PropertyAccess.EPropertyAccessCopyType
enum class EPropertyAccessCopyType : uint8_t
{
	None = 0,
	Plain = 1,
	Complex = 2,
	Bool = 3,
	Struct = 4,
	Object = 5,
	Name = 6,
	Array = 7,
	PromoteBoolToByte = 8,
	PromoteBoolToInt32 = 9,
	PromoteBoolToInt64 = 10,
	PromoteBoolToFloat = 11,
	PromoteByteToInt32 = 12,
	PromoteByteToInt64 = 13,
	PromoteByteToFloat = 14,
	PromoteInt32ToInt64 = 15,
	PromoteInt32ToFloat = 16,
	EPropertyAccessCopyType_MAX = 17
};

// Object: Enum PropertyAccess.EPropertyAccessObjectType
enum class EPropertyAccessObjectType : uint8_t
{
	None = 0,
	Object = 1,
	WeakObject = 2,
	SoftObject = 3,
	EPropertyAccessObjectType_MAX = 4
};

// Object: Enum PropertyAccess.EPropertyAccessIndirectionType
enum class EPropertyAccessIndirectionType : uint8_t
{
	Offset = 0,
	Object = 1,
	Array = 2,
	ScriptFunction = 3,
	NativeFunction = 4,
	EPropertyAccessIndirectionType_MAX = 5
};

// Package: EyeTracker
// Enums: 1

// Object: Enum EyeTracker.EEyeTrackerStatus
enum class EEyeTrackerStatus : uint8_t
{
	NotConnected = 0,
	NotTracking = 1,
	Tracking = 2,
	EEyeTrackerStatus_MAX = 3
};

// Package: MovieSceneCapture
// Enums: 2

// Object: Enum MovieSceneCapture.EHDRCaptureGamut
enum class EHDRCaptureGamut : uint8_t
{
	HCGM_Rec709 = 0,
	HCGM_P3DCI = 1,
	HCGM_Rec2020 = 2,
	HCGM_ACES = 3,
	HCGM_ACEScg = 4,
	HCGM_Linear = 5,
	HCGM_MAX = 6
};

// Object: Enum MovieSceneCapture.EMovieSceneCaptureProtocolState
enum class EMovieSceneCaptureProtocolState : uint8_t
{
	Idle = 0,
	Initialized = 1,
	Capturing = 2,
	Finalizing = 3,
	EMovieSceneCaptureProtocolState_MAX = 4
};

// Package: MoviePlayer
// Enums: 2

// Object: Enum MoviePlayer.EMoviePlaybackType
enum class EMoviePlaybackType : uint8_t
{
	MT_Normal = 0,
	MT_Looped = 1,
	MT_LoadingLoop = 2,
	MT_MAX = 3
};

// Object: Enum MoviePlayer.EMovieScreenSizeType
enum class EMovieScreenSizeType : uint8_t
{
	Default = 0,
	Stretch = 1,
	EMovieScreenSizeType_MAX = 2
};

// Package: ClothingSystemRuntimeCommon
// Enums: 2

// Object: Enum ClothingSystemRuntimeCommon.EClothingWindMethod_Legacy
enum class EClothingWindMethod_Legacy : uint8_t
{
	Legacy = 0,
	Accurate = 1,
	EClothingWindMethod_MAX = 2
};

// Object: Enum ClothingSystemRuntimeCommon.EWeightMapTargetCommon
enum class EWeightMapTargetCommon : uint8_t
{
	None = 0,
	MaxDistance = 1,
	BackstopDistance = 2,
	BackstopRadius = 3,
	AnimDriveMultiplier = 4,
	EWeightMapTargetCommon_MAX = 5
};

// Package: ClothingSystemRuntimeNv
// Enums: 1

// Object: Enum ClothingSystemRuntimeNv.EClothingWindMethodNv
enum class EClothingWindMethodNv : uint8_t
{
	Legacy = 0,
	Accurate = 1,
	EClothingWindMethodNv_MAX = 2
};

// Package: AndroidRuntimeSettings
// Enums: 8

// Object: Enum AndroidRuntimeSettings.EAndroidGraphicsDebugger
enum class EAndroidGraphicsDebugger : uint8_t
{
	None = 0,
	Mali = 1,
	Adreno = 2,
	EAndroidGraphicsDebugger_MAX = 3
};

// Object: Enum AndroidRuntimeSettings.EGoogleVRCaps
enum class EGoogleVRCaps : uint8_t
{
	Cardboard = 0,
	Daydream33 = 1,
	Daydream63 = 2,
	Daydream66 = 3,
	EGoogleVRCaps_MAX = 4
};

// Object: Enum AndroidRuntimeSettings.EGoogleVRMode
enum class EGoogleVRMode : uint8_t
{
	Cardboard = 0,
	Daydream = 1,
	DaydreamAndCardboard = 2,
	EGoogleVRMode_MAX = 3
};

// Object: Enum AndroidRuntimeSettings.EAndroidAudio
enum class EAndroidAudio : uint8_t
{
	Default = 0,
	OGG = 1,
	ADPCM = 2,
	EAndroidAudio_MAX = 3
};

// Object: Enum AndroidRuntimeSettings.EOculusMobileDevice
enum class EOculusMobileDevice : uint8_t
{
	Quest = 1,
	Quest2 = 2,
	EOculusMobileDevice_MAX = 3
};

// Object: Enum AndroidRuntimeSettings.EAndroidInstallLocation
enum class EAndroidInstallLocation : uint8_t
{
	InternalOnly = 0,
	PreferExternal = 1,
	Auto = 2,
	EAndroidInstallLocation_MAX = 3
};

// Object: Enum AndroidRuntimeSettings.EAndroidDepthBufferPreference
enum class EAndroidDepthBufferPreference : uint8_t
{
	Default = 0,
	Bits16 = 16,
	Bits24 = 24,
	Bits32 = 32,
	EAndroidDepthBufferPreference_MAX = 33
};

// Object: Enum AndroidRuntimeSettings.EAndroidScreenOrientation
enum class EAndroidScreenOrientation : uint8_t
{
	Portrait = 0,
	ReversePortrait = 1,
	SensorPortrait = 2,
	Landscape = 3,
	ReverseLandscape = 4,
	SensorLandscape = 5,
	Sensor = 6,
	FullSensor = 7,
	EAndroidScreenOrientation_MAX = 8
};

// Package: InteractiveToolsFramework
// Enums: 14

// Object: Enum InteractiveToolsFramework.EInputCaptureState
enum class EInputCaptureState : uint8_t
{
	Begin = 1,
	Continue = 2,
	End = 3,
	Ignore = 4,
	EInputCaptureState_MAX = 5
};

// Object: Enum InteractiveToolsFramework.EInputCaptureRequestType
enum class EInputCaptureRequestType : uint8_t
{
	Begin = 1,
	Ignore = 2,
	EInputCaptureRequestType_MAX = 3
};

// Object: Enum InteractiveToolsFramework.EInputCaptureSide
enum class EInputCaptureSide : uint8_t
{
	None = 0,
	Left = 1,
	Right = 2,
	Both = 3,
	Any = 99,
	EInputCaptureSide_MAX = 100
};

// Object: Enum InteractiveToolsFramework.EInputDevices
enum class EInputDevices : uint16_t
{
	None = 0,
	Keyboard = 1,
	Mouse = 2,
	Gamepad = 4,
	OculusTouch = 8,
	HTCViveWands = 16,
	AnySpatialDevice = 24,
	TabletFingers = 1024,
	EInputDevices_MAX = 1025
};

// Object: Enum InteractiveToolsFramework.ETransformGizmoSubElements
enum class ETransformGizmoSubElements : uint8_t
{
	None = 0,
	TranslateAxisX = 1,
	TranslateAxisY = 2,
	TranslateAxisZ = 3,
	TranslateAllAxes = 4,
	TranslatePlaneXY = 5,
	TranslatePlaneXZ = 6,
	TranslatePlaneYZ = 7,
	TranslateAllPlanes = 8,
	RotateAxisX = 9,
	RotateAxisY = 10,
	RotateAxisZ = 11,
	RotateAllAxes = 12,
	ScaleAxisX = 13,
	ScaleAxisY = 14,
	ScaleAxisZ = 15,
	ScaleAllAxes = 16,
	ScalePlaneYZ = 17,
	ScalePlaneXZ = 18,
	ScalePlaneXY = 19,
	ScaleAllPlanes = 20,
	ScaleUniform = 21,
	StandardTranslateRotate = 22,
	TranslateRotateUniformScale = 23,
	FullTranslateRotateScale = 24,
	ETransformGizmoSubElements_MAX = 25
};

// Object: Enum InteractiveToolsFramework.EToolChangeTrackingMode
enum class EToolChangeTrackingMode : uint8_t
{
	NoChangeTracking = 1,
	UndoToExit = 2,
	FullUndoRedo = 3,
	EToolChangeTrackingMode_MAX = 4
};

// Object: Enum InteractiveToolsFramework.EToolSide
enum class EToolSide : uint8_t
{
	Left = 0,
	Mouse = 1,
	Right = 2,
	EToolSide_MAX = 3
};

// Object: Enum InteractiveToolsFramework.EViewInteractionState
enum class EViewInteractionState : uint8_t
{
	None = 0,
	Hovered = 1,
	Focused = 2,
	EViewInteractionState_MAX = 3
};

// Object: Enum InteractiveToolsFramework.ESelectedObjectsModificationType
enum class ESelectedObjectsModificationType : uint8_t
{
	Replace = 0,
	Add = 1,
	Remove = 2,
	Clear = 3,
	ESelectedObjectsModificationType_MAX = 4
};

// Object: Enum InteractiveToolsFramework.EToolMessageLevel
enum class EToolMessageLevel : uint8_t
{
	Internal = 0,
	UserMessage = 1,
	UserNotification = 2,
	UserWarning = 3,
	UserError = 4,
	EToolMessageLevel_MAX = 5
};

// Object: Enum InteractiveToolsFramework.EToolContextCoordinateSystem
enum class EToolContextCoordinateSystem : uint8_t
{
	World = 0,
	Local = 1,
	EToolContextCoordinateSystem_MAX = 2
};

// Object: Enum InteractiveToolsFramework.EStandardToolContextMaterials
enum class EStandardToolContextMaterials : uint8_t
{
	VertexColorMaterial = 1,
	EStandardToolContextMaterials_MAX = 2
};

// Object: Enum InteractiveToolsFramework.ESceneSnapQueryTargetType
enum class ESceneSnapQueryTargetType : uint8_t
{
	None = 0,
	MeshVertex = 1,
	MeshEdge = 2,
	Grid = 4,
	All = 7,
	ESceneSnapQueryTargetType_MAX = 8
};

// Object: Enum InteractiveToolsFramework.ESceneSnapQueryType
enum class ESceneSnapQueryType : uint8_t
{
	Position = 1,
	Rotation = 2,
	ESceneSnapQueryType_MAX = 3
};

// Package: RigVM
// Enums: 5

// Object: Enum RigVM.ERigVMParameterType
enum class ERigVMParameterType : uint8_t
{
	Input = 0,
	Output = 1,
	Invalid = 2,
	ERigVMParameterType_MAX = 3
};

// Object: Enum RigVM.ERigVMOpCode
enum class ERigVMOpCode : uint8_t
{
	Execute_0_Operands = 0,
	Execute_1_Operands = 1,
	Execute_2_Operands = 2,
	Execute_3_Operands = 3,
	Execute_4_Operands = 4,
	Execute_5_Operands = 5,
	Execute_6_Operands = 6,
	Execute_7_Operands = 7,
	Execute_8_Operands = 8,
	Execute_9_Operands = 9,
	Execute_10_Operands = 10,
	Execute_11_Operands = 11,
	Execute_12_Operands = 12,
	Execute_13_Operands = 13,
	Execute_14_Operands = 14,
	Execute_15_Operands = 15,
	Execute_16_Operands = 16,
	Execute_17_Operands = 17,
	Execute_18_Operands = 18,
	Execute_19_Operands = 19,
	Execute_20_Operands = 20,
	Execute_21_Operands = 21,
	Execute_22_Operands = 22,
	Execute_23_Operands = 23,
	Execute_24_Operands = 24,
	Execute_25_Operands = 25,
	Execute_26_Operands = 26,
	Execute_27_Operands = 27,
	Execute_28_Operands = 28,
	Execute_29_Operands = 29,
	Execute_30_Operands = 30,
	Execute_31_Operands = 31,
	Execute_32_Operands = 32,
	Execute_33_Operands = 33,
	Execute_34_Operands = 34,
	Execute_35_Operands = 35,
	Execute_36_Operands = 36,
	Execute_37_Operands = 37,
	Execute_38_Operands = 38,
	Execute_39_Operands = 39,
	Execute_40_Operands = 40,
	Execute_41_Operands = 41,
	Execute_42_Operands = 42,
	Execute_43_Operands = 43,
	Execute_44_Operands = 44,
	Execute_45_Operands = 45,
	Execute_46_Operands = 46,
	Execute_47_Operands = 47,
	Execute_48_Operands = 48,
	Execute_49_Operands = 49,
	Execute_50_Operands = 50,
	Execute_51_Operands = 51,
	Execute_52_Operands = 52,
	Execute_53_Operands = 53,
	Execute_54_Operands = 54,
	Execute_55_Operands = 55,
	Execute_56_Operands = 56,
	Execute_57_Operands = 57,
	Execute_58_Operands = 58,
	Execute_59_Operands = 59,
	Execute_60_Operands = 60,
	Execute_61_Operands = 61,
	Execute_62_Operands = 62,
	Execute_63_Operands = 63,
	Execute_64_Operands = 64,
	Zero = 65,
	BoolFalse = 66,
	BoolTrue = 67,
	Copy = 68,
	Increment = 69,
	Decrement = 70,
	Equals = 71,
	NotEquals = 72,
	JumpAbsolute = 73,
	JumpForward = 74,
	JumpBackward = 75,
	JumpAbsoluteIf = 76,
	JumpForwardIf = 77,
	JumpBackwardIf = 78,
	ChangeType = 79,
	Exit = 80,
	BeginBlock = 81,
	EndBlock = 82,
	Invalid = 83,
	ERigVMOpCode_MAX = 84
};

// Object: Enum RigVM.ERigVMPinDirection
enum class ERigVMPinDirection : uint8_t
{
	Input = 0,
	Output = 1,
	IO = 2,
	Visible = 3,
	Hidden = 4,
	Invalid = 5,
	ERigVMPinDirection_MAX = 6
};

// Object: Enum RigVM.ERigVMRegisterType
enum class ERigVMRegisterType : uint8_t
{
	Plain = 0,
	String = 1,
	Name = 2,
	Struct = 3,
	Invalid = 4,
	ERigVMRegisterType_MAX = 5
};

// Object: Enum RigVM.ERigVMMemoryType
enum class ERigVMMemoryType : uint8_t
{
	Work = 0,
	Literal = 1,
	External = 2,
	Invalid = 3,
	ERigVMMemoryType_MAX = 4
};

// Package: AndroidDeviceProfileSelector
// Enums: 2

// Object: Enum AndroidDeviceProfileSelector.ECompareType
enum class ECompareType : uint8_t
{
	CMP_Equal = 0,
	CMP_Less = 1,
	CMP_LessEqual = 2,
	CMP_Greater = 3,
	CMP_GreaterEqual = 4,
	CMP_NotEqual = 5,
	CMP_Regex = 6,
	CMP_EqualIgnore = 7,
	CMP_LessIgnore = 8,
	CMP_LessEqualIgnore = 9,
	CMP_GreaterIgnore = 10,
	CMP_GreaterEqualIgnore = 11,
	CMP_NotEqualIgnore = 12,
	CMP_Hash = 13,
	CMP_MAX = 14
};

// Object: Enum AndroidDeviceProfileSelector.ESourceType
enum class ESourceType : uint8_t
{
	SRC_PreviousRegexMatch = 0,
	SRC_GpuFamily = 1,
	SRC_GlVersion = 2,
	SRC_AndroidVersion = 3,
	SRC_DeviceMake = 4,
	SRC_DeviceModel = 5,
	SRC_DeviceBuildNumber = 6,
	SRC_VulkanVersion = 7,
	SRC_UsingHoudini = 8,
	SRC_VulkanAvailable = 9,
	SRC_CommandLine = 10,
	SRC_Hardware = 11,
	SRC_Chipset = 12,
	SRC_MAX = 13
};

// Package: BinariesPatchFeature
// Enums: 1

// Object: Enum BinariesPatchFeature.EBinariesPatchFeature
enum class EBinariesPatchFeature : uint8_t
{
	None = 0,
	Count = 1,
	EBinariesPatchFeature_MAX = 2
};

// Package: PBIK
// Enums: 1

// Object: Enum PBIK.EPBIKLimitType
enum class EPBIKLimitType : uint8_t
{
	Free = 0,
	Limited = 1,
	Locked = 2,
	EPBIKLimitType_MAX = 3
};

// Package: GameMatrix
// Enums: 1

// Object: Enum GameMatrix.EGameMatrixDeviceType
enum class EGameMatrixDeviceType : uint8_t
{
	InValid = 0,
	Android = 1,
	Android_TV = 2,
	Mac = 3,
	Windows = 4,
	PC_Win = 5,
	PC_Mac = 6,
	H5 = 7,
	EGameMatrixDeviceType_MAX = 8
};

// Package: Landmass
// Enums: 2

// Object: Enum Landmass.EBrushFalloffMode
enum class EBrushFalloffMode : uint8_t
{
	Angle = 0,
	Width = 1,
	EBrushFalloffMode_MAX = 2
};

// Object: Enum Landmass.EBrushBlendType
enum class EBrushBlendType : uint8_t
{
	AlphaBlend = 0,
	Min = 1,
	Max = 2,
	Additive = 3
};

// Package: ImageWrapper
// Enums: 2

// Object: Enum ImageWrapper.EBitmapCSType
enum class EBitmapCSType : uint8_t
{
	BCST_BLCS_CALIBRATED_RGB = 0,
	BCST_LCS_sRGB = 1,
	BCST_LCS_WINDOWS_COLOR_SPACE = 2,
	BCST_PROFILE_LINKED = 3,
	BCST_PROFILE_EMBEDDED = 4,
	BCST_MAX = 5
};

// Object: Enum ImageWrapper.EBitmapHeaderVersion
enum class EBitmapHeaderVersion : uint8_t
{
	BHV_BITMAPINFOHEADER = 0,
	BHV_BITMAPV2INFOHEADER = 1,
	BHV_BITMAPV3INFOHEADER = 2,
	BHV_BITMAPV4HEADER = 3,
	BHV_BITMAPV5HEADER = 4,
	BHV_MAX = 5
};

// Package: AudioPlatformConfiguration
// Enums: 1

// Object: Enum AudioPlatformConfiguration.ESoundwaveSampleRateSettings
enum class ESoundwaveSampleRateSettings : uint8_t
{
	Max = 0,
	High = 1,
	Medium = 2,
	Low = 3,
	Min = 4,
	MatchDevice = 5
};

// Package: MediaUtils
// Enums: 4

// Object: Enum MediaUtils.EMediaPlayerOptionBooleanOverride
enum class EMediaPlayerOptionBooleanOverride : uint8_t
{
	UseMediaPlayerSetting = 0,
	Enabled = 1,
	Disabled = 2,
	EMediaPlayerOptionBooleanOverride_MAX = 3
};

// Object: Enum MediaUtils.EMediaPlayerDefinition
enum class EMediaPlayerDefinition : uint8_t
{
	Auto = 0,
	Audio = 1,
	MSD = 2,
	SD = 3,
	HD = 4,
	SHD = 5,
	EMediaPlayerDefinition_MAX = 6
};

// Object: Enum MediaUtils.EMediaPlayerOptionStreamPlayType
enum class EMediaPlayerOptionStreamPlayType : uint8_t
{
	OnlineVod = 0,
	OfflineVod = 1,
	DownloadingVod = 2,
	OnlineLive = 3,
	WillDownLoadVod = 4,
	DidDownLoadVod = 5,
	LocalFile = 6,
	ExternalUrl = 7,
	LiveExternalUrl = 8,
	EMediaPlayerOptionStreamPlayType_MAX = 9
};

// Object: Enum MediaUtils.EMediaPlayerOptionMediaFormat
enum class EMediaPlayerOptionMediaFormat : uint8_t
{
	Auto = 0,
	MultiMp4 = 1,
	OneMp4 = 2,
	HLS = 3,
	FLV = 4,
	RTMP = 5,
	DRM = 6,
	EMediaPlayerOptionMediaFormat_MAX = 7
};

// Package: BP_PCScaleType
// Enums: 1

// Object: UserDefinedEnum BP_PCScaleType.BP_PCScaleType
enum class EBP_PCScaleType : uint8_t
{
	NewEnumerator0 = 0,
	NewEnumerator1 = 1,
	NewEnumerator2 = 2,
	NewEnumerator3 = 3,
	BP_MAX = 4
};

// Package: BP_PCScaleEnableOption
// Enums: 1

// Object: UserDefinedEnum BP_PCScaleEnableOption.BP_PCScaleEnableOption
enum class EBP_PCScaleEnableOption : uint8_t
{
	NewEnumerator0 = 0,
	NewEnumerator1 = 1,
	NewEnumerator2 = 2,
	NewEnumerator3 = 3,
	BP_MAX = 4
};

// Package: BP_PreviewType
// Enums: 1

// Object: UserDefinedEnum BP_PreviewType.BP_PreviewType
enum class EBP_PreviewType : uint8_t
{
	NewEnumerator0 = 0,
	NewEnumerator1 = 1,
	NewEnumerator2 = 2,
	BP_MAX = 3
};

// Package: EWindowLayer
// Enums: 1

// Object: UserDefinedEnum EWindowLayer.EWindowLayer
enum class EWindowLayer : uint8_t
{
	NewEnumerator0 = 0,
	NewEnumerator1 = 1,
	NewEnumerator2 = 2,
	NewEnumerator5 = 3,
	NewEnumerator3 = 4,
	NewEnumerator4 = 5,
	NewEnumerator6 = 6,
	NewEnumerator7 = 7,
	NewEnumerator8 = 8,
	NewEnumerator9 = 9,
	NewEnumerator10 = 10,
	EWindowLayer_MAX = 11
};

// Package: EWindowSubLayer
// Enums: 1

// Object: UserDefinedEnum EWindowSubLayer.EWindowSubLayer
enum class EWindowSubLayer : uint8_t
{
	NewEnumerator5 = 0,
	NewEnumerator3 = 1,
	NewEnumerator4 = 2,
	NewEnumerator6 = 3,
	EWindowSubLayer_MAX = 4
};

