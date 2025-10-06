#ifndef OPEN_FLIGHT_H
#define OPEN_FLIGHT_H

#include<string>
#include<cstring>
#include<iostream>
#include<fstream>
#include<variant>
#include <memory>


using ushort = unsigned short;


ushort byteSwap(ushort value);
short  byteSwap(short value);
int    byteSwap(int value);
double byteSwap(double value);


namespace open_flight
{  

    // часть структур Record
    namespace record{
        
        enum OPCODE_ID
        {
            HEADER = 1,
            GROUP = 2,
            OBJECT = 4,
            FACE = 5
        };

        struct Header
        {
            short opcode;
            ushort length;
            char id[8];
            int formatRevision;
            int editRevision;
            char dateTime[32];
            short nextGroopId;
            short nextLODId;
            short nextObjectId;
            short nextFaceId;
            char coordinateUnits;
            char texWhite;
            char flags[4];
            int reserve1[6];
            int projectionType;
            int reserve2[7];
            short nextDOF;
            short storageType;
            int databaseOrigin;
            double southwestDatabaseCoordinateX;
            double southwestDatabaseCoordinateY;
            double deltaX;
            double deltaY;
            short nextSoundId;
            short nextPathId;
            int reserve3[2];
            short nextClipId;
            short nextTextId;
            short nextBSPId;
            short nextSwitchId;
            int reserve4[1];
            double southwestCornerLatitude;
            double southwestCornerLongitude;
            double northeastCornerLatitude;
            double northeastCornerLongitude;
            double originLatitude;
            double originLongitude;
            double lambertUpperLatitude;
            double lambertLowerLatitude;
            short nextLightSourceId;
            short nextLightPointId;
            short nextRoadId;
            short nextCATId;
            short reserve5[4];
            int earthEllipsoidModel;
            short nextAdaptiveId;
            short nextCurveId;
            short UTMZone;
            char reserve6[6];
            double deltaZ;
            double radius;
            short nextMashId;
            short nextLightPointSystemId;
            int reserve7[1];
            double earthzMajorAxis;
            double earthMinorAxis;
        };   
        
        struct Group
        {
            short opcode;
            ushort length;
            char id[8];
            short relativePriority;
            char reserve1[2];
            int flags;
            short specialEffectId1;
            short specialEffectId2;
            short significance;
            char layerCode;
            char reserve2[1];
            char reserve3[4];
            int loopCount;
            int loopDuration;
            int lastFrameDuration;
        };

        struct Object
        {
            short opcode;
            ushort length;
            char id[8];
            int flags;
            short reletivePriority;
            ushort transparency;
            short specialEffectId1;
            short specialEffectId2;   
            short significance;
            short reserve1;
        };

        struct Face
        {
            short opcode;
            ushort length;
            char id[8];
            int IRColor;
            short relativePriority;
            char drawType;
            char textureWhite;
            ushort colorNameIndex;
            ushort alternativeColorNameIndex;
            char reseve1[1];
            char templateBillboard;
            short detailTexturePatternIndex;
            short texturePatternIndex;
            short materialIndex;
            short surfaceMaterialCode;
            short futureId;
            int IRMaterialCode;
            ushort transparency;
            char LODGenerationControl;
            char lineStyleIndex;
            int flags;
            char lightMode;
            char reserve2[7];
            unsigned int packedColorPrimary;
            unsigned int packedColorAlternate;
            short textureMappingIndex;
            char reserve3[2];
            unsigned int primaryColorIndex;
            unsigned int alternateColorIndex;
            char reserve4[2];
            short shaderIndex;
        };
    }
    
// Тип variant_t представляет контейнер для хранения:
// - Указателей на различные типы записей (Header, Group, Object, Face)
// - Пустого состояния (std::monostate)
using variant_t = std::variant<
    std::unique_ptr<record::Header>, 
    std::unique_ptr<record::Group>,
    std::unique_ptr<record::Object>,
    std::unique_ptr<record::Face>,
    std::monostate
>;


// Класс Parser открывает файлы типа .flt 
// и визуализирует иерархию записей (Header, Group, Object, Face)
class Parser
{
public:
    Parser(std::string &fpath):fpath_(fpath){ }
        Parser(const char *fpath):fpath_(fpath){ }

        void start()
        {
            std::ifstream stream(fpath_, std::ios::binary);
               
            if(!stream.is_open()) {
                std::cout << "File not found." << std::endl;
                return;
            }

            is_byte_swaped = true;
            short opcode;
            short size;
            bool readed = false;
            print_info print{};
            while(!stream.eof())
            {
                if(!readed)
                    stream.read((char*)&opcode, sizeof(short));
                else{
                    opcode = byteSwap(size);
                    readed = false;
                }
                stream.read((char*)&size, sizeof(short));

                if(is_byte_swaped){
                    opcode = byteSwap(opcode);
                    size = byteSwap(size);
                }

                if(isOpcode(opcode, size)){
                    if(opcode == 10) print.level_.push_back('-');
                    else if (opcode == 11) print.level_.pop_back();

                    variant_t obj = factoryRecords(opcode, size, stream);
                    std::visit(print, obj);                    
                }
                else{
                    readed = true;
                }       
            } 
            stream.close();
        }
        bool is_byte_swaped = false;

    private:
       
        char c1_, c2_, c3_, c4_;
        std::string fpath_;
        
        // Используется в функции std::visit для обработки контейнера variant_t
        // Выводит в консоль ID записи и для Face дополнительно Material Index и Color Name Index
        struct print_info{
            void operator()(std::monostate& ptr);        
            void operator()(std::unique_ptr<record::Face>& ptr);
            void operator()(std::unique_ptr<record::Object>& ptr);
            void operator()(std::unique_ptr<record::Group>& ptr);
            void operator()(std::unique_ptr<record::Header>& ptr);
            std::string level_;
        };
        
        // При опредилении данных как opcode возвращает контейнер variant_t
        variant_t factoryRecords(short opcode, ushort size, std::ifstream& stream);
        
        // Определяет является ли данные известным opcode
        bool isOpcode(short opcode, short size);
    };
}

#endif //OPEN_FLIGHT_H