#!/bin/bash

# Parse seed from QUERY_STRING
SEED=$(echo "$QUERY_STRING" | sed 's/.*seed=\([^&]*\).*/\1/')

# Default to random seed if not provided or invalid
if ! [[ "$SEED" =~ ^[0-9]+$ ]]; then
    SEED=$$
fi

# Init PRNG with seed
RANDOM=$SEED

# SVG dimensions
W=600
H=600
N_SHAPES=30

# Generate random int between $1 and $2
rand_range() {
    local min=$1
    local max=$2
    echo $(( (RANDOM % (max - min + 1)) + min ))
}

# Generate random hex color
rand_color() {
    printf "#%02x%02x%02x" $(rand_range 30 255) $(rand_range 30 255) $(rand_range 30 255)
}

# Headers
echo "Content-Type: image/svg+xml"
echo "Status: 200"
echo ""

# SVG open
BG=$(rand_color)
echo "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"$W\" height=\"$H\" viewBox=\"0 0 $W $H\">"
echo "  <rect width=\"$W\" height=\"$H\" fill=\"$BG\"/>"

# Generate shapes
for i in $(seq 1 $N_SHAPES); do
    TYPE=$(rand_range 0 2)
    COLOR=$(rand_color)
    OPACITY=$(echo "scale=2; $(rand_range 30 90) / 100" | bc)

    if [ "$TYPE" -eq 0 ]; then
        # Circle
        CX=$(rand_range 0 $W)
        CY=$(rand_range 0 $H)
        R=$(rand_range 10 120)
        echo "  <circle cx=\"$CX\" cy=\"$CY\" r=\"$R\" fill=\"$COLOR\" opacity=\"$OPACITY\"/>"

    elif [ "$TYPE" -eq 1 ]; then
        # Rectangle
        X=$(rand_range 0 $W)
        Y=$(rand_range 0 $H)
        RW=$(rand_range 20 200)
        RH=$(rand_range 20 200)
        ROT=$(rand_range 0 360)
        echo "  <rect x=\"$X\" y=\"$Y\" width=\"$RW\" height=\"$RH\" fill=\"$COLOR\" opacity=\"$OPACITY\" transform=\"rotate($ROT $(( X + RW/2 )) $(( Y + RH/2 )))\"/>"

    else
        # Line
        X1=$(rand_range 0 $W)
        Y1=$(rand_range 0 $H)
        X2=$(rand_range 0 $W)
        Y2=$(rand_range 0 $H)
        SW=$(rand_range 1 8)
        echo "  <line x1=\"$X1\" y1=\"$Y1\" x2=\"$X2\" y2=\"$Y2\" stroke=\"$COLOR\" stroke-width=\"$SW\" opacity=\"$OPACITY\"/>"
    fi
done

echo "</svg>"