import pandas as pd
import openpyxl

# Caminho do seu arquivo CSV
caminho_csv = 'C:/Users/55119/Desktop/Facul/Atividades/3Sem/Estrutura de Dados/Trabalho/SoccerPlayerAttributes/dataset_limpo.csv'

# Ler o arquivo CSV
df = pd.read_csv(caminho_csv)

# Seleciona as primeiras 50 linhas
df_top50 = df.head(50)

# Exporta para Excel (certifique-se de ter o openpyxl instalado)
df_top50.to_excel('top_50_linhas.xlsx', index=False)
