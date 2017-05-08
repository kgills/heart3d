library(reshape2)
library(ggplot2)
library(fastICA)

# Search through all of the CSV files in the current directory 
file.names <- dir(getwd(), pattern =".csv")

ma <- function(x,n=5){filter(x,rep(1/n,n), sides=2)}

for(i in 1:length(file.names)) {
    # Import the data
    data = read.csv(file.names[i], header=TRUE, comment.char="#")

    # Get the file name without the extension
    file_sub = tools::file_path_sans_ext(file.names[i])

    data$b = ma(data$b)
    data$g = ma(data$g)
    data$r = ma(data$r)

    data=na.omit(data)

    b_scatter = ggplot(data, aes(x=t, y=b)) +
    geom_line()

    g_scatter = ggplot(data, aes(x=t, y=g)) +
    geom_line()

    r_scatter = ggplot(data, aes(x=t, y=r)) +
    geom_line()

    # Save the plots
    png(filename=paste(file_sub ,"_b_time",".png", sep=""), width=800, height=600)
    print(b_scatter)
    dev.off()

    png(filename=paste(file_sub ,"_g_time",".png", sep=""), width=800, height=600)
    print(g_scatter)
    dev.off()

    png(filename=paste(file_sub ,"_r_time",".png", sep=""), width=800, height=600)
    print(r_scatter)
    dev.off()

    # Do the fastICA
    data_t = data$t
    data$t <- NULL
    data_ica=fastICA(data, n.comp=3)
    # data_ica=data_ica$S
    data$b=data_ica$S[,1]
    data$g=data_ica$S[,2]
    data$r=data_ica$S[,3]
    data$t = data_t

    dt = data$t[2] - data$t[1]
    data$t = (data$t/dt / length(data$t)) * (1000/dt) * 0.5

    b_fft = fft(data$b)
    b_fft_mag = ma(sqrt(Im(data$b)^2 + Re(data$b)^2), 5)
    g_fft = fft(data$g)
    g_fft_mag = ma(sqrt(Im(data$g)^2 + Re(data$g)^2), 5)
    r_fft = fft(data$r)
    r_fft_mag = ma(sqrt(Im(data$r)^2 + Re(data$r)^2), 5)

    data$b=b_fft_mag
    data$g=g_fft_mag
    data$r=r_fft_mag

    b_max=which(diff(sign(diff(data$b)))==-2)+1
    g_max=which(diff(sign(diff(data$g)))==-2)+1
    r_max=which(diff(sign(diff(data$r)))==-2)+1

    print("b_max")
    for(i in b_max) {
        if(data$t[i] < 4) {
            cat(data$t[i])
            cat(" ")
            print(data$t[i]*60)
        }
    }

    print("g_max")
    for(i in g_max) {
        if(data$t[i] < 4) {
            cat(data$t[i])
            cat(" ")
            print(data$t[i]*60)
        }
    }

    print("r_max")
    for(i in r_max) {
        if(data$t[i] < 4) {
            cat(data$t[i])
            cat(" ")
            print(data$t[i]*60)
        }
    }

    b_scatter = ggplot(data, aes(x=t, y=b)) +
        geom_line()

    g_scatter = ggplot(data, aes(x=t, y=g)) +
        geom_line()

    r_scatter = ggplot(data, aes(x=t, y=r)) +
        geom_line()

    # Save the plots
    png(filename=paste(file_sub ,"_b_freq",".png", sep=""), width=800, height=600)
    print(b_scatter)
    dev.off()

    png(filename=paste(file_sub ,"_g_freq",".png", sep=""), width=800, height=600)
    print(g_scatter)
    dev.off()

    png(filename=paste(file_sub ,"_r_freq",".png", sep=""), width=800, height=600)
    print(r_scatter)
    dev.off()
}