import pandas as pd

try:
    # Caminho do seu arquivo CSV
    caminho_csv = 'C:/Users/55119/Desktop/Facul/Atividades/3Sem/Estrutura de Dados/Trabalho/SoccerPlayerAttributes/dataset_limpo3.csv'

    # Ler o arquivo CSV
    df = pd.read_csv(caminho_csv)


    pd.set_option('display.max_columns', None)

    print(df.head())
    print(df.tail())

except FileNotFoundError:
    print("Erro: Arquivo não encontrado. Verifique o caminho e nome do arquivo.")
except Exception as e:
    print(f"Erro ao carregar o arquivo: {e}")