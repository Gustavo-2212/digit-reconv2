from bokeh.plotting import figure, show, output_file


def gera_grafico():
    output_file('./tmp/erro_quadratico.html')
    
    with open("./tmp/erros.txt") as arq:
        linhas = arq.readlines()
        E = linhas[1].split(' ')
        ciclos = [x for x in range(len(E))]
        
        figura_erro = figure(title="Erro Quadrático", x_axis_label='ciclos', y_axis_label='erro', width=800, tools=["pan","box_zoom","wheel_zoom","hover","save","reset"])
        figura_erro.line(ciclos, E, legend_label="Erro Quadrático", color="red", line_width=5)
        
        show(figura_erro)
        
    return None        
