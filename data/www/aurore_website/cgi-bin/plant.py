#!/usr/bin/env python3
import os

visits = int(os.environ.get("SESSION_VISITS", "1"))

# Stade de croissance (plafond à 10)
stage = min(visits, 10)

# Hauteur de la tige proportionnelle au stade
stem_height = 20 + stage * 18
stem_y_top = 300 - stem_height

# Feuilles qui apparaissent progressivement
leaves = ""
if stage >= 2:
    leaves += '<ellipse cx="140" cy="{}" rx="30" ry="12" fill="#4CAF50" transform="rotate(-30 140 {})"/>'.format(
        stem_y_top + 60, stem_y_top + 60)
if stage >= 4:
    leaves += '<ellipse cx="160" cy="{}" rx="30" ry="12" fill="#388E3C" transform="rotate(30 160 {})"/>'.format(
        stem_y_top + 30, stem_y_top + 30)
if stage >= 6:
    leaves += '<ellipse cx="135" cy="{}" rx="35" ry="13" fill="#4CAF50" transform="rotate(-40 135 {})"/>'.format(
        stem_y_top + 10, stem_y_top + 10)

# Fleur au stade max
flower = ""
if stage >= 9:
    flower = '''
    <circle cx="150" cy="{top}" r="18" fill="#FFC107"/>
    <circle cx="150" cy="{top}" r="8" fill="#FF8F00"/>
    '''.format(top=stem_y_top - 10)
elif stage >= 7:
    flower = '<circle cx="150" cy="{}" r="10" fill="#A5D6A7"/>'.format(stem_y_top - 5)

stage_label = [
    "Une graine...", "Elle germe !", "Une petite tige",
    "Ca pousse !", "Belles feuilles", "Elle grandit vite",
    "Presque adulte", "Un bourgeon !", "Elle va fleurir...",
    "FLEUR ECLOSES !", "Plante majestueuse !"
][stage]

svg = """Content-Type: text/html\r\n\r\n
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Ma Plante</title>
  <style>
    body {{ background: #1a1a2e; display: flex; flex-direction: column;
           align-items: center; justify-content: center; height: 100vh;
           font-family: monospace; color: #e0e0e0; }}
    h2 {{ color: #A5D6A7; }}
    p  {{ color: #888; font-size: 0.9em; }}
  </style>
</head>
<body>
  <h2>{label}</h2>
  <svg width="300" height="350" xmlns="http://www.w3.org/2000/svg">
    <!-- Pot -->
    <polygon points="110,320 190,320 180,350 120,350" fill="#8D6E63"/>
    <rect x="105" y="310" width="90" height="14" rx="4" fill="#A1887F"/>
    <!-- Terre -->
    <ellipse cx="150" cy="312" rx="42" ry="6" fill="#5D4037"/>
    <!-- Tige -->
    <line x1="150" y1="312" x2="150" y2="{stem_top}"
          stroke="#558B2F" stroke-width="5" stroke-linecap="round"/>
    <!-- Feuilles -->
    {leaves}
    <!-- Fleur -->
    {flower}
  </svg>
  <p>Visite n°{visits} — encore {remaining} visites pour la floraison !</p>
</body>
</html>
""".format(
    label=stage_label,
    stem_top=stem_y_top,
    leaves=leaves,
    flower=flower,
    visits=visits,
    remaining=max(0, 9 - visits)
)

print(svg)