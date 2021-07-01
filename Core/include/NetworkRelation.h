//
// Created by bacox on 20-05-21.
//

#ifndef EDGECAFFE_NETWORKRELATION_H
#define EDGECAFFE_NETWORKRELATION_H

namespace EdgeCaffe {

    enum class RelationOperator {
        GT,
        LT,
        GEQ,
        LEQ,
        EQ
    };

    struct NetworkCondition {
        RelationOperator op;
        double value;
        std::string valueName;
    };

    struct NetworkRelation {
        std::string depends;
        NetworkCondition condition;
        bool isSet{false};
    };

    struct DataLabel {
        std::string name;
        double value;
        bool isSet{false};
    };

    inline RelationOperator stringToRelOp(const std::string& str) {
        if(str == "gt") return RelationOperator::GT;
        if(str == "lt") return RelationOperator::LT;
        if(str == "geq") return RelationOperator::GEQ;
        if(str == "leq") return RelationOperator::LEQ;
        // Default
        return RelationOperator::EQ;
    }
}

#endif //EDGECAFFE_NETWORKRELATION_H
