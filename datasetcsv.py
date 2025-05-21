import pandas as pd

# Caminho do seu arquivo CSV
caminho_csv = r'C:\Users\Eve\Desktop\Repositorio ETD\SoccerPlayerAttributes\dataset_limpo.csv'

# Ler o arquivo CSV
df = pd.read_csv(caminho_csv)

# Mostrar as primeiras 5 linhas
print(df)